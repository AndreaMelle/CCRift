//
//  DummyPopupView.m
//  CCRiftPlugin
//
//  Created by Andrea Melle on 29/09/2015.
//
//

#import "DummyPopupView.h"

@implementation DummyPopupView

-(NSMenuItem*)MenuItem
{
    return nsMenuItem;
}

-(void)OnMenuSelection:(id)sender
{
    nsMenuItem = sender;
}
@end