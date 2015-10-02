#include "CCRiftGLFWPreviewDevice.h"
#include "Eigen/Core"
#include "nanogui/window.h"
#include "nanogui/label.h"
#include "nanogui/button.h"
#include "nanogui/layout.h"
#include "nanogui/checkbox.h"

#ifdef CCRIFT_MAC
#include "CocoaUtils.h"
#endif

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
//#ifdef IS_PLUGIN
//#ifdef CCRIFT_MSW
//		HWND h = glfwGetWin32Window(window);
//		SetFocus(h);
//		SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
//#endif
//#endif
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
	#ifdef CCRIFT_MSW

	HWND h = glfwGetWin32Window(window);
	SetFocus(h);
	SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	//int count;
	//GLFWmonitor** monitors = glfwGetMonitors(&count);

	//if (IsWindow(mParentWindow))// && count >= 2)
	//{
		
		// Only handling the left case
		/*RECT r;
		if(GetWindowRect(mParentWindow, &r))
		{
			glfwSetWindowPos(window, r.right, r.top);
		}*/
		

		/*HMONITOR mNativeParentMonitor = MonitorFromWindow(mParentWindow, MONITOR_DEFAULTTONEAREST);

		for (int i = 0; i < count; i++)
		{
			HMONITOR m = (HMONITOR)(*(monitors + i));
			if(m != mNativeParentMonitor)
			{
				MONITORINFO target;
				target.cbSize = sizeof(MONITORINFO);
				GetMonitorInfo(m, &target);
				RECT r = target.rcWork;
				glfwSetWindowPos(window, r.right, r.top);
			}
		}*/

	//}

	
	LONG_PTR style = GetWindowLongPtr(h, GWL_STYLE);
	SetWindowLongPtr(h, GWL_STYLE, style & ~WS_SYSMENU);
    #else
        #ifdef CCRIFT_MAC
    CocoaChangeWindowOrder(glfwGetCocoaWindow(window), kCGMaximumWindowLevelKey);
    CocoaHideWindowCloseButton(glfwGetCocoaWindow(window), true);
        #endif
	#endif
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
	
	popupMenu->setLayout(new nanogui::GroupLayout());

	nanogui::Button *bAbout = new nanogui::Button(popupMenu, "About");
	nanogui::Button *bReset = new nanogui::Button(popupMenu, "Reset");
	nanogui::CheckBox *cGrid = new nanogui::CheckBox(popupMenu, "Grid");
	nanogui::CheckBox *cTop = new nanogui::CheckBox(popupMenu, "Always On Top");

	cGrid->setChecked(mScene->getSphere()->Grid());
	cTop->setChecked(mAlwaysOnTop);

	bAbout->setCallback([&](){
		contextualMenuCallback(CONTEXTUAL_MENU_ABOUT);
		popupMenu->setVisible(false);
	});

	bReset->setCallback([&](){
		contextualMenuCallback(CONTEXTUAL_MENU_RESET);
		popupMenu->setVisible(false);
	});

	cGrid->setCallback([&](bool check){
		contextualMenuCallback(CONTEXTUAL_MENU_GRIDTOGGLE);
		popupMenu->setVisible(false);
	});

	cTop->setCallback([&](bool check){
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
#ifdef CCRIFT_MSW
			HWND h = glfwGetWin32Window(window);
			MessageBoxA(h, "v0.1\n\nSeptember 2015\n\nandrea.melle@happyfinish.com", "About", MB_ICONINFORMATION | MB_OK);
#else
#ifdef CCRIFT_MAC
            CocoaShowMessagePopup("v0.1\n\nSeptember 2015\n\nandrea.melle@happyfinish.com");
#endif
#endif
		}
		else if (sel == CONTEXTUAL_MENU_GRIDTOGGLE)
		{
			mScene->getSphere()->toggleGrid();

		}
		else if (sel == CONTEXTUAL_MENU_ALWAYSONTOP)
		{
#ifdef IS_PLUGIN
#ifdef CCRIFT_MSW
			HWND h = glfwGetWin32Window(window);
			SetFocus(h);
			if (mAlwaysOnTop)
			{
				SetWindowPos(h, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				SetWindowPos(h, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			else
				SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#else
#ifdef CCRIFT_MAC

            if (mAlwaysOnTop)
            {
                CocoaChangeWindowOrder(glfwGetCocoaWindow(window), kCGBaseWindowLevelKey);
            }
            else
            {
                CocoaChangeWindowOrder(glfwGetCocoaWindow(window), kCGMaximumWindowLevelKey);
            }
#endif
            mAlwaysOnTop = !mAlwaysOnTop;
#endif

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

//#ifdef CCRIFT_MSW
//		HMENU hPopupMenu = CreatePopupMenu();
//		InsertMenuW(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, CONTEXTUAL_MENU_RESET, L"Reset");
//
//		UINT gridFlags = MF_BYPOSITION | MF_STRING | (mScene->getSphere()->Grid() ? MF_CHECKED : 0);
//		InsertMenuW(hPopupMenu, 1, gridFlags, CONTEXTUAL_MENU_GRIDTOGGLE, L"Grid");
//#ifdef IS_PLUGIN
//		UINT ontopFlags = MF_BYPOSITION | MF_STRING | (mAlwaysOnTop ? MF_CHECKED : 0);
//		InsertMenuW(hPopupMenu, 2, ontopFlags, CONTEXTUAL_MENU_ALWAYSONTOP, L"Always On Top");
//#endif
//		InsertMenuW(hPopupMenu, 3, MF_SEPARATOR | MF_BYPOSITION, 0, NULL);
//		InsertMenuW(hPopupMenu, 4, MF_BYPOSITION | MF_STRING, CONTEXTUAL_MENU_ABOUT, L"About");
//
//		HWND h = glfwGetWin32Window(window);
//		RECT rcWindow, rcClient;
//		GetWindowRect(h, &rcWindow);
//		GetClientRect(h, &rcClient);
//
//		int ptDiff = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
//
//		int sel = TrackPopupMenuEx(hPopupMenu,
//			TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
//			rcWindow.left + (int)mXPos,
//			rcWindow.top + (int)mYPos + ptDiff, h, NULL);
//
//		contextualMenuCallback((ContextualMenuOptions)sel);
//
//		DestroyMenu(hPopupMenu);
//#else
//#ifdef CCRIFT_MAC
//		NSWindow* h = glfwGetCocoaWindow(window);
//		HandleRButtonDown(h, xpos, ypos);
//#endif
//#endif
	//}
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