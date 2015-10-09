//
//  BitmapLoader.m
//  StandalonePreview
//
//  Created by Andrea Melle on 01/10/2015.
//  Copyright © 2015 Andrea Melle. All rights reserved.
//

#import "CocoaUtils.h"


#import <Foundation/Foundation.h>


@interface CocoaUtils : NSObject
+ (CCRift::FrameTexture*)LoadBitmap:(NSString*)file;
+ (void)ShowMessagePopup:(NSString*) msg;
@end

@implementation CocoaUtils

+ (CCRift::FrameTexture*) LoadBitmap:(NSString*)file
{
#ifdef IS_PLUGIN
    NSBundle* myBundle = [NSBundle bundleWithIdentifier:@"com.yourcompany.CCRiftPlugin"];
    NSString* bundlepath = [myBundle pathForResource:@"gridUpsideDown" ofType:@"BMP"];
    NSImage * image = [[NSImage alloc] initWithContentsOfFile:bundlepath];
#else
    NSImage * image = [[NSImage alloc] initWithContentsOfFile:file];
#endif
    
    if(!image) return nullptr;
    
    NSBitmapImageRep* bitmap = [NSBitmapImageRep alloc];
    
    if(!bitmap) return nullptr;
    
    int bpp = 0;
    NSSize imgSize = [image size];
    
    [image lockFocus];
    [bitmap initWithFocusedViewRect: NSMakeRect(0.0, 0.0, imgSize.width, imgSize.height)];
    [image unlockFocus];
    
    //glPixelStorei(GL_UNPACK_ROW_LENGTH, (GLint)[bitmap pixelsWide]);
    //glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    
    bpp = 8 * (int)[bitmap samplesPerPixel];
    unsigned int width = (unsigned int)[bitmap pixelsWide];
    unsigned int height = (unsigned int)[bitmap pixelsHigh];
    
    CCRift::FrameTexture* tex = nullptr;
    
    if (bpp == 24)
        tex = new CCRift::FrameTexture(glm::ivec2(width, height), false, false, 1, (unsigned char*)[bitmap bitmapData]);
    else if (bpp == 32)
        tex = new CCRift::FrameTexture(glm::ivec2(width, height), true, false, 1, (unsigned char*)[bitmap bitmapData]);

    
    [bitmap release];
    
    return tex;
}

+ (void)ShowMessagePopup:(NSString*) msg
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"OK"];
    [alert setMessageText:msg];
    //[alert setInformativeText:@"Deleted records cannot be restored."];
    [alert setAlertStyle:NSInformationalAlertStyle];
    
    if ([alert runModal] == NSAlertFirstButtonReturn)
    {
        
    }
    
    [alert release];
}

@end

void CocoaSetWindowAlwaysOnTop(GLFWwindow* window)
{
    NSWindow* h = (NSWindow*)glfwGetCocoaWindow(window);
    //h.level = kCGMaximumWindowLevelKey;
}

void CocoaResetWindowAlwaysOnTop(GLFWwindow* window)
{
    NSWindow* h = (NSWindow*)glfwGetCocoaWindow(window);
    //h.level = kCGBaseWindowLevelKey;
}

void CocoaHideWindowCloseButton(GLFWwindow* window)
{
    NSWindow* h = (NSWindow*)glfwGetCocoaWindow(window);
    [[h standardWindowButton:NSWindowCloseButton] setHidden:YES];
}

CCRift::FrameTexture* CocoaLoadBitmap(const char* filename)
{
    return [CocoaUtils LoadBitmap:[NSString stringWithUTF8String:filename]];
}

void CocoaShowMessagePopup(GLFWwindow* window, const char* msg, const char* title)
{
    [CocoaUtils ShowMessagePopup:[NSString stringWithUTF8String:msg]];
}

//[[glfwGetCocoaWindow(window) standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
//[[glfwGetCocoaWindow(window) standardWindowButton:NSWindowZoomButton] setHidden:YES];



