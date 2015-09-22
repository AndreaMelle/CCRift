#include <Kernel/OVR_System.h>
#include "OVR_CAPI_GL.h"
#include "CCRiftCommons.h"
#include "CCRiftOGLPlatform.h"
#include "CCRiftScene.h"

using namespace OVR;
using namespace CCRift;

struct UIThreadData
{
	Scene* scene;
	HINSTANCE hinst;
	bool uiThreadRunning;
	bool uiInited;
};



//UI thread
DWORD WINAPI UIThread(LPVOID lpParam)
{
	UIThreadData* threadData = ((UIThreadData*)lpParam);
	HINSTANCE hinst = threadData->hinst;

	OGLPlatform* context = new OGLPlatform();

	ovrGraphicsLuid luid;
	ovrSizei windowSize = { 1920 / 2, 1080 / 2 };

	VALIDATE(context->InitWindow(hinst, L"Preview"), "Failed to open window.");

	if (!context->InitDevice(windowSize.w, windowSize.h, reinterpret_cast<LUID*>(&luid)))
		return (-1);

	Scene* roomScene = threadData->scene;

	threadData->uiInited = roomScene->Init(*context, windowSize);

	while (context->HandleMessages())
	{
		roomScene->Loop(*context);
	}

	roomScene->Release(*context);

	delete roomScene;

	context->ReleaseDevice();

	delete context;

	threadData->uiThreadRunning = false;

	return 0;
}


int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	HANDLE uiThreadHandle = 0;

	UIThreadData* threadData = new UIThreadData();
	threadData->hinst = hinst;
	threadData->scene = new Scene();
	threadData->uiThreadRunning = true;
	threadData->uiInited = false;

	uiThreadHandle = CreateThread(NULL, 0, UIThread, threadData, 0, NULL);

	if (uiThreadHandle == NULL)
		ExitProcess(0);

	int dataSize = 200 * 100 * 4;
	unsigned char *data = new unsigned char[dataSize];

	memset(data, 0, dataSize);

	while (threadData->uiThreadRunning)
	{
		OutputDebugString(L"Hello output\n");

		for (int i = 0; i < dataSize; i++)
		{
			data[i] = rand() % 255;
		}

		if (threadData->scene && threadData->uiInited)
			threadData->scene->CopyFrameData(data);
		Sleep(1000);
	}

	WaitForSingleObject(uiThreadHandle, INFINITE);
	CloseHandle(uiThreadHandle);

	return 0;
}
