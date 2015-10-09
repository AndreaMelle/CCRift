//
//  BitmapLoader.h
//  StandalonePreview
//
//  Created by Andrea Melle on 01/10/2015.
//  Copyright © 2015 Andrea Melle. All rights reserved.
//

#include "CCRiftCommons.h"
#import "CCRiftFrameTexture.h"

#ifdef CCRIFT_MAC

extern "C"
{
    CCRift::FrameTexture* CocoaLoadBitmap(const char* filename);
    
    void CocoaShowMessagePopup(GLFWwindow* window, const char* msg, const char* title);
    
    void CocoaSetWindowAlwaysOnTop(GLFWwindow* window);
    void CocoaResetWindowAlwaysOnTop(GLFWwindow* window);
    void CocoaHideWindowCloseButton(GLFWwindow* window);
}

#endif
