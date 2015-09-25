#ifndef __CCRIFT_IDEVICE_H__
#define __CCRIFT_IDEVICE_H__

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

	class IDevice
	{
	public:
		IDevice() {}
		virtual ~IDevice() {}

		virtual void start(HINSTANCE hinst = NULL) = 0;
		virtual void stop() = 0;

		virtual int preferredFrameWidth() const = 0;
		virtual int preferredFrameHeight() const = 0;
		virtual size_t preferredFrameDepth() const = 0;
		virtual ovrSizei preferredFrameSize() const = 0;

		virtual int windowWidth() const = 0;
		virtual int windowHeight() const = 0;
		virtual ovrSizei windowSize() const = 0;

		virtual bool isRunning() const = 0;

		virtual void pushFrame(const void* data) = 0;
		virtual void setActive(bool active) = 0;

	};
}

#endif //__CCRIFT_IDEVICE_H__