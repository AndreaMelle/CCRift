#include "CCRiftWinPreviewDevice.h"

using namespace std;
using namespace CCRift;

WinPreviewDevice::WinPreviewDevice()
	: mWindowSize(OVR::Sizei(960, 540))
	, mFrameSize(OVR::Sizei(1920, 960))
	, mFrameBufferDepth(4)
{
	mFrameBufferLength = mFrameSize.w * mFrameSize.h * mFrameBufferDepth;
	mFrameDataBuffer = new unsigned char[mFrameBufferLength];

	onMouseDownMouseX = 0;
	onMouseDownMouseY = 0;
	lon = 0;
	onMouseDownLon = 0;
	lat = 0;
	onMouseDownLat = 0;
	mMouseSensitivity = 0.1f;
	wasDown = false;

	mAspectRatio = (float)mWindowSize.w / (float)mWindowSize.h;
}

WinPreviewDevice::~WinPreviewDevice()
{
	this->stop();
	delete[] mFrameDataBuffer;
}

void WinPreviewDevice::pushFrame(const void* data)
{
	mFrameDataMutex.lock();

	memcpy(mFrameDataBuffer, (unsigned char*)data, mFrameBufferLength);
	mFrameDataNewFlag = true;

	mFrameDataMutex.unlock();
}

void WinPreviewDevice::start(HINSTANCE hinst)
{
	mModuleHandle = hinst;

	mProcess.mThreadCallback = [&]()
	{
		HRESULT hr;
		hr = deviceSetup();

		if (FAILED(hr))
		{
			deviceTeardown();
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
	};

	mProcess.start();

}

void WinPreviewDevice::stop()
{
	mProcess.stop();
}


HRESULT WinPreviewDevice::deviceSetup()
{
	mScene = new Scene();
	mContext = new OGLPlatform();

	if (!mContext->InitWindow(mModuleHandle, L"CCRift Panorama Preview"))
	{
		MessageBoxA(NULL, "Failed to open window.", "CCRift Preview", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}

	if (!mContext->InitDevice(mWindowSize.w, mWindowSize.h, reinterpret_cast<LUID*>(&mLuid)))
	{
		MessageBoxA(NULL, "Failed to initialize OpenGL context.", "CCRift Preview", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}
	
	if (!mScene->init(mWindowSize, mFrameSize))
	{
		MessageBoxA(NULL, "Failed to initialize Scene.", "CCRift Preview", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}

	// Turn off vsync to let the compositor do its magic
	//wglSwapIntervalEXT(0);

	return S_OK;
}

HRESULT WinPreviewDevice::deviceUpdate()
{
	if (mContext->HandleMessages())
	{
		mFrameDataMutex.lock();
		mScene->updateFrameTexture(mFrameDataBuffer);
		mFrameDataNewFlag = false;
		mFrameDataMutex.unlock();

		//TODO: this totally does not account for mouse going out the window!!!!
		if (mContext->MouseDown && !wasDown)
		{
			onMouseDownMouseX = (float)mContext->MouseX;
			onMouseDownMouseY = (float)mContext->MouseY;
			onMouseDownLon = lon;
			onMouseDownLat = lat;
			wasDown = true;
		}
		else if (wasDown && mContext->MouseDown)
		{
			lon = (onMouseDownMouseX - mContext->MouseX) * mMouseSensitivity + onMouseDownLon;
			lat = (mContext->MouseY - onMouseDownMouseY) * mMouseSensitivity + onMouseDownLat;
		}
		else
		{
			wasDown = false;
		}

		float phi = (90.0f - lat) * M_PI / 180.0f;
		float theta = lon * M_PI / 180.0f;

		float targetX = 500.0f * sin(phi) * cos(theta);
		float targetY = 500.0f * cos(phi);
		float targetZ = 500.0f * sin(phi) * sin(theta);

		Matrix4f view = Matrix4f::LookAtRH(Vector3f(0, 0, 0), Vector3f(targetX, targetY, targetZ), Vector3f(0, 1, 0));

		float verticalFovRadians = 60.0f * 3.14159265f / 180.0f;
		ovrFovPort fov;

		fov.DownTan = tan(verticalFovRadians / 2);
		fov.UpTan = fov.DownTan;
		fov.LeftTan = mAspectRatio * fov.DownTan;
		fov.RightTan = fov.LeftTan;

		Matrix4f proj = ovrMatrix4f_Projection(fov, 0.2f, 1000.0f, ovrProjection_RightHanded);

		mScene->render(view, proj);

		SwapBuffers(mContext->hDC);
	}
	else
	{
		mProcess.mRunning = false;
	}

	return S_OK;
}

HRESULT WinPreviewDevice::deviceTeardown()
{
	mScene->release();
	mContext->ReleaseDevice();
	
	delete mScene;
	delete mContext;

	return S_OK;
}

///

Process::Process() : mRunning(atomic<bool>(false)), mThreadCallback(nullptr)
{

}

Process::~Process()
{
	this->stop();
}

void Process::start()
{
	this->stop();

	if (mThreadCallback != nullptr)
	{
		mRunning = true;
		mThread = shared_ptr<thread>(new thread(mThreadCallback));
	}
}

void Process::stop()
{
	mRunning = false;
	if (mThread)
	{
		mThread->join();
		mThread.reset();
	}
}