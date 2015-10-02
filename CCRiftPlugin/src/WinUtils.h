#ifndef __WIN_UTILS_H__
#define __WIN_UTILS_H__

#include "CCRiftCommons.h"

#ifdef CCRIFT_MSW

#include "CCRiftFrameTexture.h"

extern "C"
{
	CCRift::FrameTexture* WinLoadBitmap(DWORD resourcename);

	void WinShowMessagePopup(GLFWwindow* window, const char* msg, const char* title);

	void WinSetWindowAlwaysOnTop(GLFWwindow* window);
	void WinResetWindowAlwaysOnTop(GLFWwindow* window);

	void WinHideWindowCloseButton(GLFWwindow* window);
}

#endif

#endif