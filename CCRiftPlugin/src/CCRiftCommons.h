#ifndef __CCRIFT_COMMONS_H__
#define __CCRIFT_COMMONS_H__

#include <PrSDKTransmit.h>
#include <PrSDKPlayModuleAudioSuite.h>
#include <PrSDKPPixSuite.h>
#include <PrSDKSequenceInfoSuite.h>
#include <PrSDKThreadedWorkSuite.h>
#include "SDK_File.h"
#include "resource.h"
#include "CCRiftFrameTexture.h"

#ifdef PRWIN_ENV
#include <ctime>
#endif

#ifndef VALIDATE
#define VALIDATE(x, msg) if (!(x)) { MessageBoxA(NULL, (msg), "CCRift Preview", MB_ICONERROR | MB_OK); exit(-1); }
#endif

#define	PLUGIN_DISPLAY_NAME	L"CCRift Preview"

#define M_PI 3.14159265f

typedef struct
{
	csSDK_int32 mVersion;
} SDKSettings;

typedef struct
{
	csSDK_int32 mVersion;
} SDKDevicePtr;

typedef struct
{
	SPBasicSuite*					SPBasic;
	PrSDKPlayModuleAudioSuite*		PlayModuleAudioSuite;
	PrSDKPPixSuite*					PPixSuite;
	PrSDKSequenceInfoSuite*			SequenceInfoSuite;
	PrSDKThreadedWorkSuiteVersion3*	ThreadedWorkSuite;
	PrSDKTimeSuite*					TimeSuite;
} SDKSuites;

static HMODULE GetMyModuleHandle()
{
	static int s_somevar = 0;
	MEMORY_BASIC_INFORMATION mbi;
	if (!::VirtualQuery(&s_somevar, &mbi, sizeof(mbi)))
	{
		return NULL;
	}
	return static_cast<HMODULE>(mbi.AllocationBase);
}

static CCRift::FrameTexture* loadBMPFromResource(DWORD resourcename)
{
	HBITMAP hBMP;
	BITMAP  BMP;

	hBMP = (HBITMAP)LoadImage(GetMyModuleHandle(), MAKEINTRESOURCE(resourcename), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (!hBMP)
	{
		return nullptr;
	}

	GetObject(hBMP, sizeof(BMP), &BMP);

	WORD bpp = BMP.bmBitsPixel;
	unsigned int width = BMP.bmWidth;
	unsigned int height = BMP.bmHeight;

	CCRift::FrameTexture* tex = nullptr;
	
	if (bpp == 24)
		tex = new CCRift::FrameTexture(OVR::Sizei(width, height), false, false, 1, (unsigned char*)BMP.bmBits);
	else if (bpp == 32)
		tex = new CCRift::FrameTexture(OVR::Sizei(width, height), true, false, 1, (unsigned char*)BMP.bmBits);
		

	DeleteObject(hBMP);

	return tex;
}




#endif //__CCRIFT_COMMONS_H__s