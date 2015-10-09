//
//  DummyPopupView.h
//  CCRiftPlugin
//
//  Created by Andrea Melle on 29/09/2015.
//
//

#include "CCRiftCommons.h"

#ifdef CCRIFT_MAC
#import <Cocoa/Cocoa.h>

@interface DummyPopupView : NSView
{
    NSMenuItem* nsMenuItem;
}

-(void) OnMenuSelection:(id)sender;
-(NSMenuItem*)MenuItem;

@end
#endif