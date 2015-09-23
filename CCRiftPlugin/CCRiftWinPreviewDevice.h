#ifndef __CCRIFT_WINPREVIEWDEVICE_H__
#define __CCRIFT_WINPREVIEWDEVICE_H__

#include <Kernel/OVR_System.h>
#include "OVR_CAPI_GL.h"
#include "CCRiftCommons.h"
#include "CCRiftScene.h"
#include "CCRiftOGLPlatform.h"
#include <memory>
#include <thread>
#include <mutex>

namespace CCRift
{
	class Process
	{
	public:
		Process();
		virtual ~Process();

		void start();
		void stop();

		std::function<void()> mThreadCallback;
		std::atomic<bool> mRunning;
		std::shared_ptr<std::thread> mThread;
	};

	class WinPreviewDevice
	{
	public:
		WinPreviewDevice();
		~WinPreviewDevice();

		void start(HINSTANCE hinst = NULL);
		void stop();

		int preferredFrameWidth() const { return mFrameSize.w; }
		int preferredFrameHeight() const { return mFrameSize.h; }
		size_t preferredFrameDepth() const { return mFrameBufferDepth; }
		ovrSizei preferredFrameSize() const { return mFrameSize; }

		int windowWidth() const { return mWindowSize.w; }
		int windowHeight() const { return mWindowSize.h; }
		ovrSizei windowSize() const { return mWindowSize; }

		bool isRunning() const { return mProcess.mRunning; }

		void pushFrame(const void* data);

	private:

		Scene* mScene;
		OGLPlatform* mContext;
		ovrGraphicsLuid mLuid;
		ovrSizei mWindowSize;
		float mAspectRatio;
		ovrSizei mFrameSize;
		HINSTANCE mModuleHandle; //is this the 'module' in win32 lingo?

		Process mProcess;

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
	};
}

#endif //__CCRIFT_WINPREVIEWDEVICE_H__