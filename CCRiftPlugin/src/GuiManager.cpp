#include "GuiManager.h"
#include "nanovg.h"
#include "PlatformUtils.h"

using namespace nanogui;
using namespace CCRift;

void GuiManager::create(int winWidth, int winHeight)
{
	mSplashscreen = new nanogui::Window(this, "");
	mSplashscreen->setLayout(new nanogui::BoxLayout(nanogui::BoxLayout::Vertical,
		nanogui::BoxLayout::Middle, 10, 0));

	splashScreenSize = Vector2i(winWidth / 4, winHeight / 5);
	splashScreenPosition = (Vector2i(winWidth, winHeight) - splashScreenSize) / 2;

	mSplashscreen->setFixedSize(splashScreenSize);
	mSplashscreen->setPosition(splashScreenPosition);
    mSplashscreen->setUseCustomBackgroundColor(true);
    mSplashscreen->setCustomBackgroundColor(Color(38, 255));
    
	new nanogui::Label(mSplashscreen, "OmniPreview Transmitter Plugin", "sans-bold");
	new nanogui::Label(mSplashscreen, gVersion + " - " + gYear, "sans-bold");

	popupMenu = new nanogui::Window(this, "");
	popupMenu->setCustomBackgroundColor(Color(38, 255));
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
		gPreviewPresets[PP_SMARTPHONE].name,
        gPreviewPresets[PP_EDITING_DEFAULT].name
	});

	fovComboBox->popup()->setAnchorHeight(120);

	nanogui::Widget *customFovContainer = new nanogui::Widget(popupMenu);
	customFovContainer->setLayout(new nanogui::BoxLayout(nanogui::BoxLayout::Horizontal,
		nanogui::BoxLayout::Middle, 0, 4));

	slider = new nanogui::Slider(customFovContainer);
	slider->setFixedWidth(110);
	textBox = new nanogui::TextBox(customFovContainer);
	textBox->setUnits("\xc2\xb0");
	textBox->setFixedSize(nanogui::Vector2i(60, 25));

	fovComboBox->setCallback([&](int value) {
		switch (value)
		{
		case 0:
            mPPChangedCallback(PP_SMARTPHONE);
			break;
		case 1:
            mPPChangedCallback(PP_EDITING_DEFAULT);
			break;
		default:
			break;
		}
	});

	slider->setCallback([&](float value) {
		textBox->setValue(std::to_string((int)(std::max<float>(FOV_MINIMUM, value * FOV_MAXIMUM))));
	});
	slider->setFinalCallback([&](float value) {
        mSliderChangeCallback(std::max<float>(FOV_MINIMUM, value * FOV_MAXIMUM));
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
    
    showCustomSettingsBox(false);

}

void GuiManager::showCustomSettingsBox(bool show)
{
    slider->setEnabled(show);
    slider->setVisible(show);
    textBox->setVisible(show);
}

void GuiManager::showSplashScreen(bool show)
{
	mSplashscreen->setVisible(show);
}

void GuiManager::didResize(int winWidth, int winHeight)
{
	mSettingsBtn->setPosition(nanogui::Vector2i(20, winHeight - 20 - mSettingsBtn->width()));
	
    //splashScreenSize = Vector2i(winWidth / 2, winHeight / 2 );
    splashScreenPosition = (Vector2i(winWidth, winHeight) - splashScreenSize) / 2;
	mSplashscreen->setPosition(splashScreenPosition);
//    mSplashscreen->setFixedSize(splashScreenSize);
//    mLogo->setFixedSize(splashScreenSize - nanogui::Vector2i(60,60));
    performLayout(nvgContext());
    
}

void GuiManager::setPresetOption(PreviewPreset option)
{
    switch (option)
    {
        case PP_SMARTPHONE:
            fovComboBox->setSelectedIndex(0);
            break;
        case PP_EDITING_DEFAULT:
            fovComboBox->setSelectedIndex(1);
            break;
        default:
            break;
    }
}

void GuiManager::setCustomEditingOptions(PreviewSettings settings)
{
    textBox->setValue(std::to_string((int)(settings.fov)));
    slider->setValue(settings.fov / FOV_MAXIMUM);
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
