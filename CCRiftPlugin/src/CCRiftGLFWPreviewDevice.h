#ifndef __CCRIFT_GLFWPREVIEWDEVICE_H__
#define __CCRIFT_GLFWPREVIEWDEVICE_H__

#include "CCRiftCommons.h"
#include "CCRiftScene.h"
#include "CCRiftOGLPlatform.h"
#include "CCRiftIDevice.h"

namespace CCRift
{
	class GLFWPreviewDevice : public IDevice<GLFWPreviewDevice>
	{
		friend IDevice;
	public:
		
		virtual ~GLFWPreviewDevice();

#ifdef CCRIFT_MSW
		void setMainWindowHandle(HWND handle)
		{
			mParentWindow = handle;
		};
#endif

		virtual void start(HINSTANCE hinst = NULL) override;
		virtual void stop() override;

		virtual int preferredFrameWidth() const override { return mFrameSize.x; }
		virtual int preferredFrameHeight() const override { return mFrameSize.y; }
		virtual size_t preferredFrameDepth() const override { return mFrameBufferDepth; }
        virtual glm::ivec2 preferredFrameSize() const override { return mFrameSize; }

		int windowWidth() const { return mWindowSize.x; }
		int windowHeight() const { return mWindowSize.y; }
		glm::ivec2 windowSize() const { return mWindowSize; }

		virtual bool isRunning() const override { return mDeviceRunning; }

		virtual void pushFrame(const void* data) override;
		virtual void setActive(bool active) override;

		void glfwErrorCallback(int error, const char* description);
		void glfwKeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
		void glfwResizeCallback(GLFWwindow* w, int width, int height);

	protected:
		GLFWPreviewDevice();
		GLFWwindow* window;

		Scene* mScene;
		glm::ivec2 mWindowSize;
		glm::ivec2 mFrameSize;

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
		bool mAlwaysOnTop;
		bool mActive;

        glm::vec3 handleMouseInput();

		float verticalFovDegrees;
		float mAspectRatio;
        glm::mat4 mProj;

		std::function<void(ContextualMenuOptions)> contextualMenuCallback;

#ifdef CCRIFT_MSW
		HWND mParentWindow;
#endif


	};

	static void CreateContextualMenu(std::function<void(ContextualMenuOptions)>& contextualMenuCallback)
	{

	}

}

#endif //__CCRIFT_GLFWPREVIEWDEVICE_H__