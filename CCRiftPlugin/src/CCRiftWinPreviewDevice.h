#ifndef __CCRIFT_WINPREVIEWDEVICE_H__
#define __CCRIFT_WINPREVIEWDEVICE_H__

#include "CCRiftCommons.h"

#ifdef OCULUS_RIFT_PLATFORM

#include "CCRiftScene.h"
#include "CCRiftOGLPlatform.h"
#include "CCRiftIDevice.h"

namespace CCRift
{
	class WinPreviewDevice : public IDevice<WinPreviewDevice>
	{
		friend IDevice;
	public:
		virtual ~WinPreviewDevice();

		virtual void start(HINSTANCE hinst = NULL) override;
		virtual void stop() override;

		virtual int preferredFrameWidth() const override { return mFrameSize.w; }
		virtual int preferredFrameHeight() const override { return mFrameSize.h; }
		virtual size_t preferredFrameDepth() const override { return mFrameBufferDepth; }
		virtual glm::ivec2 preferredFrameSize() const override { return mFrameSize; }

		virtual int windowWidth() const override { return mWindowSize.w; }
		virtual int windowHeight() const override { return mWindowSize.h; }
		virtual glm::ivec2 windowSize() const override { return mWindowSize; }

		virtual bool isRunning() const override { return mDeviceRunning; }

		virtual void pushFrame(const void* data) override;
		virtual void setActive(bool active) override;

	protected:
		WinPreviewDevice();

		Scene* mScene;
		OGLPlatform* mContext;
		ovrGraphicsLuid mLuid;
		glm::ivec2 mWindowSize;
		glm::ivec2 mFrameSize;
		HINSTANCE mModuleHandle; //is this the 'module' in win32 lingo?

		Process mProcess;

		std::atomic<bool> mDeviceRunning;

		HRESULT deviceSetup();
		HRESULT deviceUpdate();
		HRESULT deviceTeardown();

		unsigned char *mFrameDataBuffer;
		size_t mFrameBufferLength;
		size_t mFrameBufferDepth; //in bytes
		std::atomic<bool> mFrameDataNewFlag;
		std::mutex mFrameDataMutex;

		float onMouseDownMouseX;
		float onMouseDownMouseY;
		float lon;
		float onMouseDownLon;
		float lat;
		float onMouseDownLat;
		float mMouseSensitivity;
		bool wasDown;

		bool mActive;

		Vector3f handleMouseInput();

		float mAspectRatio;
		ovrFovPort mFov;
		Matrix4f mProj;

	};
}

#endif //__CCRIFT_WINPREVIEWDEVICE_H__
#endif