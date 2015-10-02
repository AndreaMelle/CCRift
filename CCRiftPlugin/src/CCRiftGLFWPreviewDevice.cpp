#include "CCRiftGLFWPreviewDevice.h"
#include "Eigen/Core"
#include "nanogui/window.h"
#include "nanogui/label.h"
#include "nanogui/button.h"
#include "nanogui/layout.h"
#include "nanogui/checkbox.h"

#include "PlatformUtils.h"

using namespace std;
using namespace CCRift;

GLFWPreviewDevice::GLFWPreviewDevice()
: mWindowSize(glm::ivec2(960, 540))
    , mFrameSize(glm::ivec2(1920, 960))

	, mDeviceRunning(false)
    , mFrameBufferDepth(4)
    , mXPos(0)
    , mYPos(0)
    , onMouseDownMouseX(0)
    , onMouseDownMouseY(0)
    , lon(0)
    , onMouseDownLon(0)
    , lat(0)
    , onMouseDownLat(0)
    , mMouseSensitivity(0.1f)
    , wasDown(false)
#ifdef CCRIFT_MSW
	, mParentWindow(0)
#endif
    , mAlwaysOnTop(false)
    , mActive(false)
    , verticalFovDegrees(60.0f)
    , contextualMenuCallback([](ContextualMenuOptions){})
{
	mFrameBufferLength = mFrameSize.x * mFrameSize.y * mFrameBufferDepth;
	mFrameDataBuffer = new unsigned char[mFrameBufferLength];

	mAspectRatio = (float)mWindowSize.x / (float)mWindowSize.y;
	float verticalFovRadians = verticalFovDegrees * M_PI / 180.0f;
	mProj = glm::perspectiveFovRH(verticalFovRadians, (float)mWindowSize.x, (float)mWindowSize.y, 0.1f, 100.0f); //RH
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
    
#ifndef IS_PLUGIN
    #ifdef CCRIFT_MAC
    
    HRESULT hr;
    hr = deviceSetup();
    
    if (FAILED(hr))
    {
        deviceTeardown();
        mDeviceRunning = false;
        return;
    }
    
    size_t demoDataSize = preferredFrameHeight()
    * preferredFrameWidth()
    * preferredFrameDepth();
    
    unsigned char *demoData = new unsigned char[demoDataSize];
    
    memset(demoData, 0, demoDataSize);
        
        for (int i = 0; i < demoDataSize; i++)
        {
            demoData[i] = rand() % 255;
        }

    pushFrame(demoData);
    
    mProcess.mRunning = true;
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
    #endif
#endif
    
	mProcess.start();

}

void GLFWPreviewDevice::stop()
{
	if (mProcess.mRunning)
		mProcess.stop();
}


HRESULT GLFWPreviewDevice::deviceSetup()
{
	glfwSetErrorCallback([](int error, const char* description){
		//printf(description);
		IDevice<GLFWPreviewDevice>::Instance().glfwErrorCallback(error, description);
	});

	if (!glfwInit())
	{
		//MessageBoxA(NULL, "Failed to initialize OpenGL context.", "CCRift Preview", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}
	
#ifdef CCRIFT_MAC
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#else
#ifdef CCRIFT_MSW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
#endif
	
	glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

	window = glfwCreateWindow(mWindowSize.x, mWindowSize.y, "CCRift Panorama Preview", NULL, NULL);

	if (!window)
	{
		//MessageBoxA(NULL, "Failed to open window.", "CCRift Preview", MB_ICONERROR | MB_OK);
		glfwTerminate();
		return E_FAIL;
	}

	glfwMakeContextCurrent(window);
    
#ifdef CCRIFT_MSW
	glewExperimental = GL_TRUE;
#endif
    
    if(glewInit() != GLEW_OK)
    {
        //printf("Failed to initialize GLEW\n");
        return E_FAIL;
    }
    
	glfwSwapInterval(1);

#ifdef IS_PLUGIN
	SetWindowAlwaysOnTop(window);
	HideWindowCloseButton(window);
    mAlwaysOnTop = true;
#endif
	//glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
	
	mScene = new Scene();

	if (!mScene->init(mWindowSize, mFrameSize))
	{
		//MessageBoxA(NULL, "Failed to initialize Scene.", "CCRift Preview", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}

	mGUI = new nanogui::Screen(window);
	popupMenu = new nanogui::Window(mGUI, "");
	
	nanogui::GroupLayout *layout = new nanogui::GroupLayout();
	//layout->setSpacing(0);
	//layout->setMargin(0);
	popupMenu->setLayout(layout);

	nanogui::Button *bAbout = new nanogui::Button(popupMenu, "About");
	bAbout->setWidth(popupMenu->width());

	nanogui::Button *bReset = new nanogui::Button(popupMenu, "Reset");
	nanogui::Button *cGrid = new nanogui::Button(popupMenu, "Grid");
	nanogui::Button *cTop = new nanogui::Button(popupMenu, "Always On Top");

	cGrid->setButtonFlags(nanogui::Button::ToggleButton);
	cTop->setButtonFlags(nanogui::Button::ToggleButton);

	cGrid->setPushed(mScene->getSphere()->Grid());
	cTop->setPushed(mAlwaysOnTop);

	bAbout->setCallback([&](){
		contextualMenuCallback(CONTEXTUAL_MENU_ABOUT);
		popupMenu->setVisible(false);
	});

	bReset->setCallback([&](){
		contextualMenuCallback(CONTEXTUAL_MENU_RESET);
		popupMenu->setVisible(false);
	});

	cGrid->setChangeCallback([&](bool check){
		contextualMenuCallback(CONTEXTUAL_MENU_GRIDTOGGLE);
		popupMenu->setVisible(false);
	});

	cTop->setChangeCallback([&](bool check){
		contextualMenuCallback(CONTEXTUAL_MENU_ALWAYSONTOP);
		popupMenu->setVisible(false);
	});

	mGUI->performLayout(mGUI->nvgContext());

	glfwSetCursorPosCallback(window, [](GLFWwindow *w, double x, double y){
		IDevice<GLFWPreviewDevice>::Instance().glfwCursorPosCallback(w, x, y);
	});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* w, int button, int action, int modifiers) {
		IDevice<GLFWPreviewDevice>::Instance().glfwMouseButtonCallback(w, button, action, modifiers);
	});

	glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
		IDevice<GLFWPreviewDevice>::Instance().glfwKeyCallback(w, key, scancode, action, mods);
	});

	glfwSetScrollCallback(window, [](GLFWwindow* w, double x, double y) {
		IDevice<GLFWPreviewDevice>::Instance().glfwScrollCallback(w, x, y);
	});

	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		IDevice<GLFWPreviewDevice>::Instance().glfwResizeCallback(window, width, height);

	});
	
#ifdef IS_PLUGIN
	glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
		glfwSetWindowShouldClose(window, GL_FALSE);
	});
#endif

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
			ShowMessagePopup(window, "v0.1\n\nSeptember 2015\n\nandrea.melle@happyfinish.com", "About");
		}
		else if (sel == CONTEXTUAL_MENU_GRIDTOGGLE)
		{
			mScene->getSphere()->toggleGrid();

		}
		else if (sel == CONTEXTUAL_MENU_ALWAYSONTOP)
		{
#ifdef IS_PLUGIN
			if (mAlwaysOnTop)
				ResetWindowAlwaysOnTop(window);
			else
				SetWindowAlwaysOnTop(window);
            mAlwaysOnTop = !mAlwaysOnTop;
#endif
		}
        
	};

	// Turn off vsync to let the compositor do its magic
	//wglSwapIntervalEXT(0);

	mGUI->setVisible(true);
	popupMenu->setVisible(false);


	return S_OK;
}

glm::vec3 GLFWPreviewDevice::handleMouseInput()
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	
	if (wasDown && state == GLFW_PRESS)
	{
		lon = (onMouseDownMouseX - (float)mXPos) * mMouseSensitivity + onMouseDownLon;
		lat = ((float)mYPos - onMouseDownMouseY) * mMouseSensitivity + onMouseDownLat;
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glCullFace(GL_FRONT);

		mScene->render(view, mProj);
		mGUI->render();

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
	delete mGUI;

	//if (popupMenu)
	//	delete popupMenu;

	glfwDestroyWindow(window);
	glfwTerminate();

	return S_OK;
}

void GLFWPreviewDevice::glfwCursorPosCallback(GLFWwindow* w, double x, double y)
{
	if (window != w) return;

	mXPos = x;
	mYPos = y;

	mGUI->onCursorPos(w, x, y);
}

void GLFWPreviewDevice::glfwMouseButtonCallback(GLFWwindow* w, int button, int action, int modifiers)
{
	if (w != window) return;

	if (mGUI->onMouseButton(w, button, action, modifiers)) return;

	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

	if (state == GLFW_PRESS)
	{
		popupMenu->setVisible(false);
	}

	if (state == GLFW_PRESS && !wasDown)
	{
		onMouseDownMouseX = (float)mXPos;
		onMouseDownMouseY = (float)mYPos;
		onMouseDownLon = lon;
		onMouseDownLat = lat;
		wasDown = true;
	}

	state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

	if (state == GLFW_PRESS)
	{
		popupMenu->setVisible(true);
		popupMenu->setPosition(Eigen::Vector2i(mXPos, mYPos));
	}
}

void GLFWPreviewDevice::glfwScrollCallback(GLFWwindow* w, double x, double y)
{
	if (window != w) return;
	mGUI->onScroll(w, x, y);
}

void GLFWPreviewDevice::glfwResizeCallback(GLFWwindow* w, int width, int height)
{
	if (window != w) return;

	mWindowSize.x = width;
	mWindowSize.y = height;

	mAspectRatio = (float)mWindowSize.x / (float)mWindowSize.y;
	float verticalFovRadians = verticalFovDegrees * M_PI / 180.0f;
	mProj = glm::perspectiveFovRH(verticalFovRadians, (float)mWindowSize.x, (float)mWindowSize.y, 0.1f, 100.0f); //RH
	glViewport(0, 0, mWindowSize.x, mWindowSize.y);

	mGUI->onResize(w, width, height);
}

void GLFWPreviewDevice::glfwKeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods)
{
#ifndef IS_PLUGIN
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		this->stop();
	}
#endif

	mGUI->onKey(w, key, scancode, action, mods);

}

void GLFWPreviewDevice::glfwErrorCallback(int error, const char* description)
{

}