#include <Kernel/OVR_System.h>
#include "OVR_CAPI_GL.h"
#include "CCRiftCommons.h"
#include "CCRiftOGLPlatform.h"
#include "CCRiftScene.h"

using namespace OVR;
using namespace CCRift;

HANDLE ghMutex;
bool uiThreadRunning;

//UI thread
DWORD WINAPI UIThread(LPVOID lpParam)
{
	HINSTANCE hinst = *((HINSTANCE*)lpParam);

	OGLPlatform* context = new OGLPlatform();

	ovrGraphicsLuid luid;
	ovrSizei windowSize = { 1920 / 2, 1080 / 2 };

	VALIDATE(context->InitWindow(hinst, L"Preview"), "Failed to open window.");

	if (!context->InitDevice(windowSize.w, windowSize.h, reinterpret_cast<LUID*>(&luid)))
		return (-1);

	Scene* roomScene = nullptr;
	roomScene = new Scene();

	roomScene->Init(*context, windowSize);

	while (context->HandleMessages())
	{
		roomScene->Loop(*context);
	}

	roomScene->Release(*context);

	delete roomScene;

	context->ReleaseDevice();

	delete context;

	uiThreadRunning = false;

	return 0;
}


int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	HANDLE uiThreadHandle = 0;

	uiThreadHandle = CreateThread(NULL, 0, UIThread, &hinst, 0, NULL);

	if (uiThreadHandle == NULL)
		ExitProcess(0);

	uiThreadRunning = true;

	while (uiThreadRunning)
	{
		OutputDebugString(L"Hello output\n");
		Sleep(1000);
	}

	WaitForSingleObject(uiThreadHandle, INFINITE);
	CloseHandle(uiThreadHandle);

	return 0;
}
