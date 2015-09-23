#ifndef __CCRIFT_COMMONS_H__
#define __CCRIFT_COMMONS_H__

#include <PrSDKTransmit.h>
#include <PrSDKPlayModuleAudioSuite.h>
#include <PrSDKPPixSuite.h>
#include <PrSDKSequenceInfoSuite.h>
#include <PrSDKThreadedWorkSuite.h>
#include "SDK_File.h"
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



#endif //__CCRIFT_COMMONS_H__s