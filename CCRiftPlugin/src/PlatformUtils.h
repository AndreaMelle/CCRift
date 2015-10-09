#ifndef __PLATFORM_UTILS__
#define __PLATFORM_UTILS__

#include "CCRiftFrameTexture.h"

extern "C"
{
	CCRift::FrameTexture* LoadTextureFromBitmap(const char* filename);

	void ShowMessagePopup(GLFWwindow* window, const char* msg, const char* title);

	void SetWindowAlwaysOnTop(GLFWwindow* window);
	void ResetWindowAlwaysOnTop(GLFWwindow* window);

	void HideWindowCloseButton(GLFWwindow* window);
}

#endif // __PLATFORM_UTILS__