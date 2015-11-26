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
        //PP_DK2,
        //PP_GEARVR,
        //PP_CARDBOARD,
        PP_EDITING_DEFAULT
    } PreviewPreset;
    
    static std::map<PreviewPreset, PreviewSettings> gPreviewPresets =
    {
        { PP_SMARTPHONE, PreviewSettings(16.0f / 9.0f, 667, 375, 75, false, "Smartphone") },
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
