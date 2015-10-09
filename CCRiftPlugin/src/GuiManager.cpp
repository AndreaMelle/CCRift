#include "GuiManager.h"

using namespace nanogui;
using namespace CCRift;

void GuiManager::create(int winWidth, int winHeight)
{
	mSplashscreen = new nanogui::Window(this, "");

	splashScreenSize = Vector2i(400, 300);
	splashScreenPosition = (Vector2i(winWidth, winHeight) - splashScreenSize) / 2;

	mSplashscreen->setFixedSize(splashScreenSize);
	mSplashscreen->setPosition(splashScreenPosition);

	popupMenu = new nanogui::Window(this, "");

	popupMenu->setFixedWidth(200);

	nanogui::GroupLayout *layout = new nanogui::GroupLayout();
	popupMenu->setLayout(layout);

	mSettingsBtn = new nanogui::Button(this, "", ENTYPO_ICON_COG);

	mSettingsBtn->setFixedSize(nanogui::Vector2i(30, 30));
	mSettingsBtn->setPosition(nanogui::Vector2i(20, winHeight - 20 - 30));
	mSettingsBtn->setFontSize(20);

	//layout->setSpacing(0);
	//layout->setMargin(0);

	new nanogui::Label(popupMenu, "General", "sans-bold");
	bAbout = new nanogui::Button(popupMenu, "About");
	bReset = new nanogui::Button(popupMenu, "Reset");
	cGrid = new nanogui::Button(popupMenu, "Grid");
	cGrid->setButtonFlags(nanogui::Button::ToggleButton);
	cTop = new nanogui::Button(popupMenu, "Always On Top");
	cTop->setButtonFlags(nanogui::Button::ToggleButton);

	new nanogui::Label(popupMenu, "Field of View", "sans-bold");

	fovComboBox = new nanogui::ComboBox(popupMenu, {
		gFovOptions[FOV_DK2].name,
		gFovOptions[FOV_GEARVR].name,
		gFovOptions[FOV_CARDBOARD].name,
		gFovOptions[FOV_HANDHELD].name,
		gFovOptions[FOV_CUSTOM_DEFAULT].name
	});

	fovComboBox->popup()->setAnchorHeight(120);

	nanogui::Widget *customFovContainer = new nanogui::Widget(popupMenu);
	customFovContainer->setLayout(new nanogui::BoxLayout(nanogui::BoxLayout::Horizontal,
		nanogui::BoxLayout::Middle, 0, 4));

	nanogui::Slider *slider = new nanogui::Slider(customFovContainer);
	slider->setFixedWidth(110);
	nanogui::TextBox *textBox = new nanogui::TextBox(customFovContainer);
	textBox->setUnits("\xc2\xb0");
	textBox->setFixedSize(nanogui::Vector2i(60, 25));

	fovComboBox->setCallback([&, slider, textBox](int value) {

		bool showSlider = false;

		switch (value)
		{
		case 0:
			//Oculus Rift DK2
			mFovChangedCallback(gFovOptions[FOV_DK2].fovDegrees);
			break;
		case 1:
			//Gear VR
			mFovChangedCallback(gFovOptions[FOV_GEARVR].fovDegrees);
			break;
		case 2:
			//Google Cardboard
			mFovChangedCallback(gFovOptions[FOV_CARDBOARD].fovDegrees);
			break;
		case 3:
			//Handheld
			mFovChangedCallback(gFovOptions[FOV_HANDHELD].fovDegrees);
			break;
		case 4:
			showSlider = true;
			mFovChangedCallback(std::max<float>(gFovOptions[FOV_MINIMUM].fovDegrees, slider->value() * gFovOptions[FOV_MAXIMUM].fovDegrees));
			break;
		default:
			break;
		}

		slider->setEnabled(showSlider);
		slider->setVisible(showSlider);
		textBox->setVisible(showSlider);

	});

	textBox->setValue(std::to_string((int)(gFovOptions[FOV_CUSTOM_DEFAULT].fovDegrees)));
	textBox->setVisible(false);
	slider->setEnabled(false);
	slider->setVisible(false);
	slider->setValue(gFovOptions[FOV_CUSTOM_DEFAULT].fovDegrees / gFovOptions[FOV_MAXIMUM].fovDegrees);

	slider->setCallback([&, textBox](float value) {
		textBox->setValue(std::to_string((int)(std::max<float>(gFovOptions[FOV_MINIMUM].fovDegrees, value * gFovOptions[FOV_MAXIMUM].fovDegrees))));
	});
	slider->setFinalCallback([&](float value) {
		mFovChangedCallback(std::max<float>(gFovOptions[FOV_MINIMUM].fovDegrees, value * gFovOptions[FOV_MAXIMUM].fovDegrees));
	});

	bAbout->setCallback([&](){
		mAboutOptionCallback();
		popupMenu->setVisible(false);
	});

	bReset->setCallback([&](){
		mResetOptionCallback();
		popupMenu->setVisible(false);
	});

	cGrid->setChangeCallback([&](bool check){
		mGridOptionCallback(check);
		popupMenu->setVisible(false);
	});

	cTop->setChangeCallback([&](bool check){
		mAlwaysOnTopOptionCallback(check);
		popupMenu->setVisible(false);
	});

	mSettingsBtn->setCallback([&]() {
		popupMenu->setVisible(true);
		int y = -5 - popupMenu->height();
		popupMenu->setPosition(mSettingsBtn->position() + nanogui::Vector2i(0, y));
	});

	performLayout(nvgContext());

	setVisible(true);
	popupMenu->setVisible(false);
	mSplashscreen->setVisible(false);

}

void GuiManager::showSplashScreen(bool show)
{
	mSplashscreen->setVisible(show);
}

void GuiManager::didResize(int winWidth, int winHeight)
{
	mSettingsBtn->setPosition(nanogui::Vector2i(20, winHeight - 20 - mSettingsBtn->width()));
	splashScreenPosition = (Vector2i(winWidth, winHeight) - splashScreenSize) / 2;
	mSplashscreen->setPosition(splashScreenPosition);
}

void GuiManager::setFovOption(FOVInfo fovInfo)
{
	//FIXME: hack
	fovComboBox->setSelectedIndex(3);
}

//TODO: do we call the callback for setters?
void GuiManager::setGridOption(bool on)
{
	cGrid->setPushed(on);
}

void GuiManager::setAlwaysOnTopOption(bool on)
{
	cTop->setPushed(on);
}

void GuiManager::dismissPopupMenu()
{
	popupMenu->setVisible(false);
}
