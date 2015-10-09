#include "WinUtils.h"

void WinSetWindowAlwaysOnTop(GLFWwindow* window)
{
	HWND h = glfwGetWin32Window(window);
	SetFocus(h);
	SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void WinResetWindowAlwaysOnTop(GLFWwindow* window)
{
	HWND h = glfwGetWin32Window(window);
	SetFocus(h);
	SetWindowPos(h, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(h, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void WinHideWindowCloseButton(GLFWwindow* window)
{
	HWND h = glfwGetWin32Window(window);
	LONG_PTR style = GetWindowLongPtr(h, GWL_STYLE);
	SetWindowLongPtr(h, GWL_STYLE, style & ~WS_SYSMENU);
}

void WinShowMessagePopup(GLFWwindow* window, const char* msg, const char* title)
{
	HWND h = glfwGetWin32Window(window);
	MessageBoxA(h, msg, title, MB_ICONINFORMATION | MB_OK);
}

CCRift::FrameTexture* WinLoadBitmap(DWORD resourcename)
{
	HBITMAP hBMP;
	BITMAP  BMP;

	hBMP = (HBITMAP)LoadImage(GetMyModuleHandle(), MAKEINTRESOURCE(resourcename), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (!hBMP)
	{
		return nullptr;
	}

	GetObject(hBMP, sizeof(BMP), &BMP);

	WORD bpp = BMP.bmBitsPixel;
	unsigned int width = BMP.bmWidth;
	unsigned int height = BMP.bmHeight;

	CCRift::FrameTexture* tex = nullptr;

	if (bpp == 24)
		tex = new CCRift::FrameTexture(glm::ivec2(width, height), false, false, 1, (unsigned char*)BMP.bmBits);
	else if (bpp == 32)
		tex = new CCRift::FrameTexture(glm::ivec2(width, height), true, false, 1, (unsigned char*)BMP.bmBits);


	DeleteObject(hBMP);

	return tex;
}


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