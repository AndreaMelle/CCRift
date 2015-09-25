#ifndef __CCRIFT_GLFWPREVIEWDEVICE_H__
#define __CCRIFT_GLFWPREVIEWDEVICE_H__

#include "CCRiftCommons.h"
#include "CCRiftScene.h"
#include "CCRiftOGLPlatform.h"
#include "CCRiftIDevice.h"

#include "GLFW\glfw3.h"
#include <stdlib.h>
#include <stdio.h>

namespace CCRift
{
	class GLFWPreviewDevice : public IDevice<GLFWPreviewDevice>
	{
		friend IDevice;
	public:
		
		virtual ~GLFWPreviewDevice();

		virtual void start(HINSTANCE hinst = NULL) override;
		virtual void stop() override;

		virtual int preferredFrameWidth() const override { return mFrameSize.w; }
		virtual int preferredFrameHeight() const override { return mFrameSize.h; }
		virtual size_t preferredFrameDepth() const override { return mFrameBufferDepth; }
		virtual ovrSizei preferredFrameSize() const override { return mFrameSize; }

		virtual int windowWidth() const override { return mWindowSize.w; }
		virtual int windowHeight() const override { return mWindowSize.h; }
		virtual ovrSizei windowSize() const override { return mWindowSize; }

		virtual bool isRunning() const override { return mDeviceRunning; }

		virtual void pushFrame(const void* data) override;
		virtual void setActive(bool active) override;

		void glfwErrorCallback(int error, const char* description);
		void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	protected:
		GLFWPreviewDevice();
		GLFWwindow* window;

		Scene* mScene;
		//GLPlatform* mContext;
		//ovrGraphicsLuid mLuid;
		ovrSizei mWindowSize;
		ovrSizei mFrameSize;
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

#endif //__CCRIFT_GLFWPREVIEWDEVICE_H__