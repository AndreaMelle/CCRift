#include "PlatformUtils.h"

#ifdef CCRIFT_MAC

#endif

#ifdef CCRIFT_MSW
#include "WinUtils.h"
#endif


CCRift::FrameTexture* LoadTextureFromBitmap(const char* filename)
{
#ifdef CCRIFT_MSW
	return WinLoadBitmap(IDB_BITMAP1);
#endif
}

void ShowMessagePopup(GLFWwindow* window, const char* msg, const char* title)
{
#ifdef CCRIFT_MSW
	WinShowMessagePopup(window, msg, title);
#endif
}

void SetWindowAlwaysOnTop(GLFWwindow* window)
{
#ifdef CCRIFT_MSW
	WinSetWindowAlwaysOnTop(window);
#endif
}

void ResetWindowAlwaysOnTop(GLFWwindow* window)
{
#ifdef CCRIFT_MSW
	WinResetWindowAlwaysOnTop(window);
#endif
}

void HideWindowCloseButton(GLFWwindow* window)
{
#ifdef CCRIFT_MSW
	WinHideWindowCloseButton(window);
#endif
}