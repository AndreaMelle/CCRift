#ifndef __CCRIFT_IDEVICE_H__
#define __CCRIFT_IDEVICE_H__

#include "CCRiftCommons.h"
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

#ifndef CCRIFT_MSW
#define HINSTANCE int*
#endif

namespace CCRift
{
	typedef enum ContextualMenuOptions
	{
		CONTEXTUAL_MENU_NONE,
		CONTEXTUAL_MENU_RESET,
		CONTEXTUAL_MENU_ABOUT,
		CONTEXTUAL_MENU_GRIDTOGGLE,
		CONTEXTUAL_MENU_ALWAYSONTOP,
		CONTEXTUAL_MENU_LAST
	} ContextualMenuOptions;

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

	template <typename T>
	class IDevice
	{
	public:
		static T& Instance()
		{
			static T instance;
			return instance;
		}

		virtual ~IDevice() {}

		virtual void start(HINSTANCE hinst = NULL) = 0;
		virtual void stop() = 0;

		virtual int preferredFrameWidth() const = 0;
		virtual int preferredFrameHeight() const = 0;
		virtual size_t preferredFrameDepth() const = 0;
        virtual glm::ivec2 preferredFrameSize() const = 0;

		

		virtual bool isRunning() const = 0;

		virtual void pushFrame(const void* data) = 0;
		virtual void setActive(bool active) = 0;

	protected:
		IDevice()  {}
		IDevice(IDevice const&) = delete;
		void operator=(IDevice const&) = delete;
	};
}

#endif //__CCRIFT_IDEVICE_H__