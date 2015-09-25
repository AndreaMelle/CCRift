#ifndef __CCRIFT_COMMONS_H__
#define __CCRIFT_COMMONS_H__

#define PREVIEW_PLATFORM

#ifdef OCULUS_RIFT_PLATFORM
	#include "GL/CAPI_GLE.h"
	#include "Extras/OVR_Math.h"
	#include "Kernel/OVR_Log.h"
	#include "OVR_CAPI_GL.h"
#else
	#ifdef PREVIEW_PLATFORM
		#include <glad/glad.h>
		#include <glm/fwd.hpp>
		#include <glm/glm.hpp>
		#include <glm/gtc/quaternion.hpp>
		#include <glm/gtc/matrix_transform.hpp>
		#include <GLFW/glfw3.h>

		#if defined(CCRIFT_CC_GNU)
		#define CCRIFT_OFFSETOF(class_, member_) ((size_t)(((uintptr_t)&reinterpret_cast<const volatile char&>((((class_*)65536)->member_))) - 65536))
		#else
		#define CCRIFT_OFFSETOF(class_, member_) offsetof(class_, member_)
		#endif

	#endif
#endif

#include <PrSDKTransmit.h>
#include <PrSDKPlayModuleAudioSuite.h>
#include <PrSDKPPixSuite.h>
#include <PrSDKSequenceInfoSuite.h>
#include <PrSDKThreadedWorkSuite.h>
#include "SDK_File.h"
#include "resource.h"

#include <vector>


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






#endif //__CCRIFT_COMMONS_H__s