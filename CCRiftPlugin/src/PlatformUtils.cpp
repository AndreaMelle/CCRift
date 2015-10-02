#include "PlatformUtils.h"

#ifdef CCRIFT_MAC
#include "CocoaUtils.h"
#endif

#ifdef CCRIFT_MSW
#include "WinUtils.h"
#endif


CCRift::FrameTexture* LoadTextureFromBitmap(const char* filename)
{
#ifdef CCRIFT_MSW
	return WinLoadBitmap(IDB_BITMAP1);
#endif
    
#ifdef CCRIFT_MAC
    return CocoaLoadBitmap(filename);
#endif
}

void ShowMessagePopup(GLFWwindow* window, const char* msg, const char* title)
{
#ifdef CCRIFT_MSW
	WinShowMessagePopup(window, msg, title);
#endif

#ifdef CCRIFT_MAC
    CocoaShowMessagePopup(window, msg, title);
#endif
}

void SetWindowAlwaysOnTop(GLFWwindow* window)
{
#ifdef CCRIFT_MSW
	WinSetWindowAlwaysOnTop(window);
#endif
    
#ifdef CCRIFT_MAC
    CocoaSetWindowAlwaysOnTop(window);
#endif
}

void ResetWindowAlwaysOnTop(GLFWwindow* window)
{
#ifdef CCRIFT_MSW
	WinResetWindowAlwaysOnTop(window);
#endif
    
#ifdef CCRIFT_MAC
    CocoaResetWindowAlwaysOnTop(window);
#endif
}

void HideWindowCloseButton(GLFWwindow* window)
{
#ifdef CCRIFT_MSW
	WinHideWindowCloseButton(window);
#endif
    
#ifdef CCRIFT_MAC
    CocoaHideWindowCloseButton(window);
#endif
}