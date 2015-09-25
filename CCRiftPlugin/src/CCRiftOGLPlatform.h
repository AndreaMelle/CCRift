#ifdef OCULUS_RIFT_PLATFORM
#ifndef __CCRIFT_OGLPLATFORM_H__
#define __CCRIFT_OGLPLATFORM_H__

#include "GL/CAPI_GLE.h"
#include "Extras/OVR_Math.h"
#include "Kernel/OVR_Log.h"
#include "OVR_CAPI_GL.h"
#include "CCRiftCommons.h"
#include <functional>

using namespace OVR;

namespace CCRift
{
	class OGLPlatform
	{
	public:

		typedef enum ContextualMenuOptions
		{
			CONTEXTUAL_MENU_NONE,
			CONTEXTUAL_MENU_RESET,
			CONTEXTUAL_MENU_ABOUT,
			CONTEXTUAL_MENU_GRIDTOGGLE,
			CONTEXTUAL_MENU_LAST
		};

		OGLPlatform();
		virtual ~OGLPlatform();

		bool InitWindow(HINSTANCE hInst, LPCWSTR title);
		void Stop();
		void CloseWindow();
		void BringWindowToFront();

		bool InitDevice(int vpW, int vpH, const LUID* /*pLuid*/, bool windowed = true);
		bool HandleMessages(void);
		//void Run(bool(*MainLoop)(bool retryCreate, OGLPlatform& context));
		void ReleaseDevice();

		static void GLAPIENTRY DebugGLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
		static LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam);

		std::function<void(ContextualMenuOptions)> contextualMenuCallback;

	public:

		static const bool       UseDebugContext = false;

		HWND                    Window;
		HDC                     hDC;
		HGLRC                   WglContext;
		OVR::GLEContext         GLEContext;
		bool                    Running;
		//bool                    Key[256];
		int						MouseX;
		int						MouseY;
		bool					MouseDown;
		bool					MouseDrag;
		int                     WinSizeW;
		int                     WinSizeH;
		//GLuint                  fboId;
		HINSTANCE               hInstance;

	};
}

#endif //__CCRIFT_OGLPLATFORM_H__
#endif