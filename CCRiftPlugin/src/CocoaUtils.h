//
//  BitmapLoader.h
//  StandalonePreview
//
//  Created by Andrea Melle on 01/10/2015.
//  Copyright © 2015 Andrea Melle. All rights reserved.
//

#include "CCRiftCommons.h"

#ifdef CCRIFT_MAC

extern "C"
{
    void* CocoaLoadBitmap(const char* filename);
    
    void CocoaShowMessagePopup(const char* msg);
    
    void CocoaChangeWindowOrder(id window, int level);
    
    void CocoaHideWindowCloseButton(id window, bool hide);
}

#endif
