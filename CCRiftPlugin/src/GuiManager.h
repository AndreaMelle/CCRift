#ifndef __GUI_MANAGER_H__
#define __GUI_MANAGER_H__

#include "CCRiftCommons.h"
#include "nanogui/screen.h"
#include "Eigen/Core"
#include "nanogui/window.h"
#include "nanogui/label.h"
#include "nanogui/button.h"
#include "nanogui/layout.h"
#include "nanogui/checkbox.h"
#include "nanogui/popup.h"
#include "nanogui/popupbutton.h"
#include "nanogui/slider.h"
#include "nanogui/textbox.h"
#include "nanogui/combobox.h"
#include "nanogui/entypo.h"
#include "CCRiftTimer.h"

namespace CCRift
{
	class GuiManager : public nanogui::Screen
	{
	public:
		GuiManager(GLFWwindow* w) : Screen(w) {}
		virtual ~GuiManager() {}

		virtual void create(int winWidth, int winHeight) override;
		virtual void didResize(int winWidth, int winHeight) override;

		void setFovChangeOptionCallback(std::function<void(float)> callback) { mFovChangedCallback = callback; }
		
		void setAboutOptionCallback(std::function<void()> callback) { mAboutOptionCallback = callback; }
		void setResetOptionCallback(std::function<void()> callback) { mResetOptionCallback = callback; }

		void setGridOptionCallback(std::function<void(bool)> callback) { mGridOptionCallback = callback; }
		void setAlwaysOnTopOptionCallback(std::function<void(bool)> callback) { mAlwaysOnTopOptionCallback = callback; }

		void setGridOption(bool on);
		void setAlwaysOnTopOption(bool on);
		void setFovOption(FOVInfo fovInfo);

		void showSplashScreen(bool show);

		void dismissPopupMenu();

	private:

		nanogui::Window *mSplashscreen;

		nanogui::Button* mSettingsBtn;
		nanogui::Window *popupMenu;
		nanogui::Button *bAbout;
		nanogui::Button *bReset;
		nanogui::Button *cGrid;
		nanogui::Button *cTop;
		nanogui::ComboBox* fovComboBox;

		nanogui::Vector2i splashScreenSize;
		nanogui::Vector2i splashScreenPosition;

		std::function<void(float)> mFovChangedCallback;

		std::function<void()> mAboutOptionCallback;
		std::function<void()> mResetOptionCallback;

		std::function<void(bool)> mGridOptionCallback;
		std::function<void(bool)> mAlwaysOnTopOptionCallback;

		

	};
}

#endif //__GUI_MANAGER_H__