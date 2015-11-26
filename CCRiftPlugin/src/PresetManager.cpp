//
//  PresetManager.cpp
//  StandalonePreview
//
//  Created by Andrea Melle on 10/10/2015.
//  Copyright © 2015 Andrea Melle. All rights reserved.
//

#include "PresetManager.h"
#include "GuiManager.h"

using namespace CCRift;

PresetManager::PresetManager(GuiManager* gui, PreviewPreset initial)
    : mGUI(gui)
{
    mLatestEditingSettings = gPreviewPresets[PP_EDITING_DEFAULT];
    
    mGUI->setPresetOption(initial);
    
    if(initial == PP_EDITING_DEFAULT)
    {
        mGUI->showCustomSettingsBox(true);
        mGUI->setCustomEditingOptions(mLatestEditingSettings);
    }
    
    mGUI->setPresetChangeOptionCallback([&](PreviewPreset value) {
        switch (value) {
            case PP_EDITING_DEFAULT:
                mGUI->showCustomSettingsBox(true);
                mGUI->setCustomEditingOptions(mLatestEditingSettings);
                mPreviewSettingsChangeCallback(mLatestEditingSettings);
                break;
            default:
                // here we turn off the custom settings box in the gui
                mGUI->showCustomSettingsBox(false);
                mPreviewSettingsChangeCallback(gPreviewPresets[value]);
                break;
        }
    });
    
    mGUI->setSliderChangeCallback([&](float value) {
        mLatestEditingSettings.fov = value;
        mPreviewSettingsChangeCallback(mLatestEditingSettings);
    });
    
}

PresetManager::~PresetManager()
{
    
}