//
//  PresetManager.hpp
//  StandalonePreview
//
//  Created by Andrea Melle on 10/10/2015.
//  Copyright © 2015 Andrea Melle. All rights reserved.
//

#ifndef PresetManager_hpp
#define PresetManager_hpp

#include "CCRiftCommons.h"
#include <functional>

namespace CCRift
{
    class GuiManager;
    
    typedef struct PreviewSettings
    {
    public:
        PreviewSettings() {}
        PreviewSettings(float aspectratioIn,
                        int widthIn,
                        int heightIn,
                        float fovIn,
                        bool isVrIn,
                        std::string nameIn)
        : aspectratio(aspectratioIn)
        , width(widthIn)
        , height(heightIn)
        , fov(fovIn)
        , isVr(isVrIn)
        , name(nameIn) {}
        
        virtual ~PreviewSettings() {}
        
        float aspectratio; // w / h
        int width; // default width in points of the settings preview (if size is locked, this cannot be changed)
        int height;
        float fov; //degrees
        bool isVr;
        std::string name;
        
    } PreviewSettings;
    
    typedef enum PreviewPreset
    {
		PP_SMARTPHONE,
		PP_SMARTPHONE_WIDE,
		PP_CARDBOARD,
        //PP_DK2,
        //PP_GEARVR,
        PP_EDITING_DEFAULT
    } PreviewPreset;
    
    static std::map<PreviewPreset, PreviewSettings> gPreviewPresets =
    {
		{ PP_SMARTPHONE, PreviewSettings(667.0f / 375.0f, 667, 375, 75, false, "Smartphone - 75\xc2\xb0") },
		{ PP_SMARTPHONE_WIDE, PreviewSettings(736.0f / 414.0f, 736, 414, 75, false, "Large Smartphone - 75\xc2\xb0") },
		{ PP_CARDBOARD, PreviewSettings(667.0f / 375.0f, 667, 375, 90, true, "Cardboard - 90\xc2\xb0") },
        { PP_EDITING_DEFAULT, PreviewSettings(4.0f / 3.0f, 960, 720, 60, false, "Editing") }
    };
    
    static const float FOV_MINIMUM = 10.0f;
    static const float FOV_MAXIMUM = 150.0f;
    
    class PresetManager
    {
    public:
        PresetManager(GuiManager* gui, PreviewPreset initial);
        virtual ~PresetManager();
        
        void setPreviewSettingsChangeCallback(std::function<void(PreviewSettings)> callback) { mPreviewSettingsChangeCallback = callback; }
        
    private:
        GuiManager* mGUI;
        
        PreviewSettings mLatestEditingSettings;
        
        std::function<void(PreviewSettings)> mPreviewSettingsChangeCallback;
        
    };
}

#endif /* PresetManager_hpp */
