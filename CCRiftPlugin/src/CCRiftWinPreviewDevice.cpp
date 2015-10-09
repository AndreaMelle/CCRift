#ifdef CCRIFT_MSW
#include "CCRiftWinPreviewDevice.h"

using namespace std;
using namespace CCRift;

WinPreviewDevice::WinPreviewDevice()
	: mWindowSize(OVR::Sizei(960, 540))
	, mFrameSize(OVR::Sizei(1920, 960))
	, mFrameBufferDepth(4)
	, mActive(false)
	, onMouseDownMouseX(0)
	, onMouseDownMouseY(0)
	, lon(0)
	, onMouseDownLon(0)
	, lat(0)
	, onMouseDownLat(0)
	, mMouseSensitivity(0.1f)
	, wasDown(false)
	, mDeviceRunning(false)
{
	mFrameBufferLength = mFrameSize.w * mFrameSize.h * mFrameBufferDepth;
	mFrameDataBuffer = new unsigned char[mFrameBufferLength];

	mAspectRatio = (float)mWindowSize.w / (float)mWindowSize.h;

	float verticalFovRadians = 60.0f * M_PI / 180.0f;

	mFov.DownTan = tan(verticalFovRadians * 0.5f);
	mFov.UpTan = mFov.DownTan;
	mFov.LeftTan = mAspectRatio * mFov.DownTan;
	mFov.RightTan = mFov.LeftTan;

	mProj = ovrMatrix4f_Projection(mFov, 0.1f, 100.0f, ovrProjection_RightHanded);
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

void WinPreviewDevice::setActive(bool active)
{
	if (active)
	{
		
		//mContext->BringWindowToFront();
	}
	else
	{
		
	}

	mActive = active;
}

void WinPreviewDevice::start(HINSTANCE hinst)
{
	if (mDeviceRunning) return;

	mDeviceRunning = true;

	mModuleHandle = hinst;

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

	mContext->contextualMenuCallback = [&](OGLPlatform::ContextualMenuOptions sel){
		if (sel == OGLPlatform::CONTEXTUAL_MENU_RESET)
		{
			lat = 0;
			lon = 0;
		}
		else if (sel == OGLPlatform::CONTEXTUAL_MENU_ABOUT)
		{
			MessageBoxA(mContext->Window, "v0.1\n\nSeptember 2015\n\nandrea.melle@happyfinish.com", "About", MB_ICONINFORMATION | MB_OK);
		}
		else if (sel == OGLPlatform::CONTEXTUAL_MENU_GRIDTOGGLE)
		{
			mScene->getSphere()->toggleGrid();
		}
	};

	// Turn off vsync to let the compositor do its magic
	//wglSwapIntervalEXT(0);

	return S_OK;
}

Vector3f WinPreviewDevice::handleMouseInput()
{
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

	Vector3f result;

	result.x = 500.0f * sin(phi) * cos(theta);
	result.y = 500.0f * cos(phi);
	result.z = 500.0f * sin(phi) * sin(theta);

	return result;
}

HRESULT WinPreviewDevice::deviceUpdate()
{
	if (mContext->HandleMessages())
	{
		mFrameDataMutex.lock();
		mScene->updateFrameTexture(mFrameDataBuffer);
		mFrameDataNewFlag = false;
		mFrameDataMutex.unlock();
		
		Vector3f target = handleMouseInput();
		Matrix4f view = Matrix4f::LookAtRH(Vector3f(0, 0, 0), target, Vector3f(0, 1, 0));

		mScene->render(view, mProj);

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
#endif