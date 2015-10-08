#ifndef __CCRIFT_GLFWPREVIEWDEVICE_H__
#define __CCRIFT_GLFWPREVIEWDEVICE_H__

#include "CCRiftCommons.h"
#include "CCRiftScene.h"
#include "CCRiftOGLPlatform.h"
#include "CCRiftIDevice.h"
#include "nanogui/screen.h"
#include <map>

namespace CCRift
{
	class GLFWPreviewDevice : public IDevice<GLFWPreviewDevice>
	{
	public:

		typedef enum FOVOption
		{
			FOV_MINIMUM,
			FOV_HANDHELD,
			FOV_DK2,
			FOV_GEARVR,
			FOV_CARDBOARD,
			FOV_CUSTOM_DEFAULT,
			FOV_MAXIMUM
		} FOVOption;

		typedef struct FOVInfo
		{
		public:
			FOVInfo() {}
			FOVInfo(FOVOption _option, float _fovDeg, std::string _name)
				: option(_option)
				, fovDegrees(_fovDeg)
				, name(_name) {}

			FOVOption option;
			float fovDegrees;
			std::string name;
		};

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
		void glfwResizeCallback(GLFWwindow* w, int width, int height);
		void glfwCursorPosCallback(GLFWwindow* w, double x, double y);
		void glfwMouseButtonCallback(GLFWwindow* w, int button, int action, int modifiers);
		void glfwKeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
		void glfwScrollCallback(GLFWwindow* w, double x, double y);

		void setFieldOfView(float vFOV);

	protected:
		GLFWPreviewDevice();
		GLFWwindow* window;

		nanogui::Screen *mGUI;
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
		nanogui::Window *popupMenu;

        glm::vec3 handleMouseInput();

		float verticalFovDegrees;
		float mAspectRatio;
        glm::mat4 mProj;

		std::function<void(ContextualMenuOptions)> contextualMenuCallback;

		static std::map<FOVOption, FOVInfo> gFovOptions;

#ifdef CCRIFT_MSW
		HWND mParentWindow;
#endif


	};

}

#endif //__CCRIFT_GLFWPREVIEWDEVICE_H__