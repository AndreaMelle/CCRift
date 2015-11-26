#ifndef __CCRIFT_GLFWPREVIEWDEVICE_H__
#define __CCRIFT_GLFWPREVIEWDEVICE_H__

#include "CCRiftCommons.h"
#include "CCRiftScene.h"
#include "CCRiftOGLPlatform.h"
#include "CCRiftIDevice.h"
#include "GuiManager.h"
#include "PresetManager.h"
#include <map>

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

		int windowWidth() const { return mCurrentSettings.width; }
		int windowHeight() const { return mCurrentSettings.height; }
        glm::ivec2 windowSize() const { return glm::ivec2(mCurrentSettings.width, mCurrentSettings.height); }

		virtual bool isRunning() const override { return mDeviceRunning; }

		virtual void pushFrame(const void* data) override;
		virtual void setActive(bool active) override;

		void glfwErrorCallback(int error, const char* description);
		void glfwResizeCallback(GLFWwindow* w, int width, int height);
		void glfwCursorPosCallback(GLFWwindow* w, double x, double y);
		void glfwMouseButtonCallback(GLFWwindow* w, int button, int action, int modifiers);
		void glfwKeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
		void glfwScrollCallback(GLFWwindow* w, double x, double y);

		
        void setPreviewSettings(PreviewSettings ps);

	protected:
		GLFWPreviewDevice();
		GLFWwindow* window;

		GuiManager *mGUI;
        PresetManager *mPresetManager;
		Timer mSplashScreenTimer;
        double mSplashScreenTimeout;
		bool mShouldShowSplashScreen;

        PreviewSettings mCurrentSettings;
        
		Scene* mScene;
		//glm::ivec2 mWindowSize;
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

		double mXPos;
		double mYPos;
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
        
        void updateProjectionMatrix();
        void setFieldOfView(float vFOV);
        void setWindowSize(int width, int height);

		//float verticalFovDegrees;
		//float mAspectRatio;
        glm::mat4 mProj;


#ifdef CCRIFT_MSW
		HWND mParentWindow;
#endif


	};

}

#endif //__CCRIFT_GLFWPREVIEWDEVICE_H__