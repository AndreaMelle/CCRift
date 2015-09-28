#include "CCRiftGLFWPreviewDevice.h"

using namespace std;
using namespace CCRift;

static void error_callback(int error, const char* description)
{
	printf(description);
	IDevice<GLFWPreviewDevice>::Instance().glfwErrorCallback(error, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	IDevice<GLFWPreviewDevice>::Instance().glfwKeyCallback(window, key, scancode, action, mods);
}

GLFWPreviewDevice::GLFWPreviewDevice()
: mWindowSize(glm::ivec2(960, 540))
    , mFrameSize(glm::ivec2(1920, 960))
	, mDeviceRunning(false)
    , mFrameBufferDepth(4)
    , onMouseDownMouseX(0)
    , onMouseDownMouseY(0)
    , lon(0)
    , onMouseDownLon(0)
    , lat(0)
    , onMouseDownLat(0)
    , mMouseSensitivity(0.1f)
    , wasDown(false)
    , mActive(false)
	
{
	mFrameBufferLength = mFrameSize.x * mFrameSize.y * mFrameBufferDepth;
	mFrameDataBuffer = new unsigned char[mFrameBufferLength];

	mAspectRatio = (float)mWindowSize.x / (float)mWindowSize.y;

	float verticalFovDegrees = 60.0f ;
	float verticalFovRadians = verticalFovDegrees * M_PI / 180.0f;

	mFov.DownTan = tan(verticalFovRadians * 0.5f);
	mFov.UpTan = mFov.DownTan;
	mFov.LeftTan = mAspectRatio * mFov.DownTan;
	mFov.RightTan = mFov.LeftTan;
    
	mProj = glm::perspectiveFovRH(verticalFovRadians, (float)mWindowSize.x, (float)mWindowSize.y, 0.1f, 100.0f); //RH
	//mProj = ovrMatrix4f_Projection(mFov, 0.1f, 100.0f, ovrProjection_RightHanded);
}

GLFWPreviewDevice::~GLFWPreviewDevice()
{
	this->stop();
	if (mFrameDataBuffer)
		delete[] mFrameDataBuffer;
}

void GLFWPreviewDevice::pushFrame(const void* data)
{
	mFrameDataMutex.lock();

	memcpy(mFrameDataBuffer, (unsigned char*)data, mFrameBufferLength);
	mFrameDataNewFlag = true;

	mFrameDataMutex.unlock();
}

void GLFWPreviewDevice::setActive(bool active)
{
	if (active)
	{
		//mContext->BringWindowToFront();
	}
	else
	{
		
	}

	mActive = active;
}

void GLFWPreviewDevice::start(HINSTANCE hinst)
{
	if (mDeviceRunning) return;

	mDeviceRunning = true;

	//mModuleHandle = hinst;

	mProcess.mThreadCallback = [&]()
	{
		HRESULT hr;
		hr = deviceSetup();

		if (FAILED(hr))
		{
			deviceTeardown();
			mDeviceRunning = false;
			return;
		}

		while (mProcess.mRunning)
		{
			hr = deviceUpdate();

			if (FAILED(hr))
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(33));
				continue;
			}
		}

		deviceTeardown();

		mDeviceRunning = false;
	};
    
    
    //glfwSetErrorCallback(error_callback);
    //
    //if (!glfwInit())
    //{
    //    //MessageBoxA(NULL, "Failed to initialize OpenGL context.", "CCRift Preview", MB_ICONERROR | MB_OK);
    //    return;
    //}
    //
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //
    //window = glfwCreateWindow(mWindowSize.x, mWindowSize.y, "CCRift Panorama Preview", NULL, NULL);
    //
    //if (!window)
    //{
    //    //MessageBoxA(NULL, "Failed to open window.", "CCRift Preview", MB_ICONERROR | MB_OK);
    //    glfwTerminate();
    //    return;
    //}

	mProcess.start();

}

void GLFWPreviewDevice::stop()
{
	if (mProcess.mRunning)
		mProcess.stop();
}


HRESULT GLFWPreviewDevice::deviceSetup()
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		//MessageBoxA(NULL, "Failed to initialize OpenGL context.", "CCRift Preview", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(mWindowSize.x, mWindowSize.y, "CCRift Panorama Preview", NULL, NULL);


	if (!window)
	{
		//MessageBoxA(NULL, "Failed to open window.", "CCRift Preview", MB_ICONERROR | MB_OK);
		glfwTerminate();
		return E_FAIL;
	}

	glfwMakeContextCurrent(window);

	//glewExperimental = GL_TRUE;

    if(glewInit() != GLEW_OK)
    {
        //printf("Failed to initialize GLEW\n");
        return E_FAIL;
    }
    
	glfwSwapInterval(1);

	glfwSetKeyCallback(window, key_callback);
	//glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
	
	mScene = new Scene();

	if (!mScene->init(mWindowSize, mFrameSize))
	{
		//MessageBoxA(NULL, "Failed to initialize Scene.", "CCRift Preview", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);

	contextualMenuCallback = [&](ContextualMenuOptions sel){
		if (sel == CONTEXTUAL_MENU_RESET)
		{
			lat = 0;
			lon = 0;
		}
		else if (sel == CONTEXTUAL_MENU_ABOUT)
		{
#ifdef CCRIFT_MSW
			HWND h = glfwGetWin32Window(window);
			MessageBoxA(h, "v0.1\n\nSeptember 2015\n\nandrea.melle@happyfinish.com", "About", MB_ICONINFORMATION | MB_OK);
#endif
		}
		else if (sel == CONTEXTUAL_MENU_GRIDTOGGLE)
		{
			mScene->getSphere()->toggleGrid();
		}
	};

	// Turn off vsync to let the compositor do its magic
	//wglSwapIntervalEXT(0);

	return S_OK;
}

glm::vec3 GLFWPreviewDevice::handleMouseInput()
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

	//TODO: this totally does not account for mouse going out the window!!!!
	if (state == GLFW_PRESS && !wasDown)
	{
		onMouseDownMouseX = (float)xpos;
		onMouseDownMouseY = (float)ypos;
		onMouseDownLon = lon;
		onMouseDownLat = lat;
		wasDown = true;
	}
	else if (wasDown && state == GLFW_PRESS)
	{
		lon = (onMouseDownMouseX - (float)xpos) * mMouseSensitivity + onMouseDownLon;
		lat = ((float)ypos - onMouseDownMouseY) * mMouseSensitivity + onMouseDownLat;
	}
	else
	{
		wasDown = false;
	}

	float phi = (90.0f - lat) * M_PI / 180.0f;
	float theta = lon * M_PI / 180.0f;

	glm::vec3 result;

	result.x = 500.0f * sin(phi) * cos(theta);
	result.y = 500.0f * cos(phi);
	result.z = 500.0f * sin(phi) * sin(theta);

	state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

	if (state == GLFW_PRESS)
	{
#ifdef CCRIFT_MSW
		HMENU hPopupMenu = CreatePopupMenu();
		InsertMenuW(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, CONTEXTUAL_MENU_RESET, L"Reset");
		InsertMenuW(hPopupMenu, 1, MF_BYPOSITION | MF_STRING, CONTEXTUAL_MENU_GRIDTOGGLE, L"Toggle Grid");
		InsertMenuW(hPopupMenu, 2, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
		InsertMenuW(hPopupMenu, 3, MF_BYPOSITION | MF_STRING, CONTEXTUAL_MENU_ABOUT, L"About");
		
		HWND h = glfwGetWin32Window(window);
		RECT rcWindow, rcClient;
		GetWindowRect(h, &rcWindow);
		GetClientRect(h, &rcClient);

		int ptDiff = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;

		int sel = TrackPopupMenuEx(hPopupMenu,
			TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
			rcWindow.left + (int)xpos,
			rcWindow.top + (int)ypos + ptDiff, h, NULL);

		contextualMenuCallback((ContextualMenuOptions)sel);

		DestroyMenu(hPopupMenu);
#endif
	}

	return result;
}

HRESULT GLFWPreviewDevice::deviceUpdate()
{
	if (!glfwWindowShouldClose(window))
	{
		/*glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;*/

		mFrameDataMutex.lock();
		mScene->updateFrameTexture(mFrameDataBuffer);
		mFrameDataNewFlag = false;
		mFrameDataMutex.unlock();
		
		glm::vec3 target = handleMouseInput();
        glm::mat4 view = glm::lookAtRH(glm::vec3(0, 0, 0), target, glm::vec3(0, 1, 0));

		//glViewport(0, 0, mWindowSize.x, mWindowSize.y);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);

		mScene->render(view, mProj);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	else
	{
		mProcess.mRunning = false;
	}

	return S_OK;
}

HRESULT GLFWPreviewDevice::deviceTeardown()
{
	mScene->release();
	delete mScene;

	glfwDestroyWindow(window);
	glfwTerminate();

	return S_OK;
}

void GLFWPreviewDevice::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		this->stop();
	}
}

void GLFWPreviewDevice::glfwErrorCallback(int error, const char* description)
{

}