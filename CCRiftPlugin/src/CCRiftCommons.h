#ifndef __CCRIFT_COMMONS_H__
#define __CCRIFT_COMMONS_H__

#include <PrSDKTransmit.h>
#include <PrSDKPlayModuleAudioSuite.h>
#include <PrSDKPPixSuite.h>
#include <PrSDKSequenceInfoSuite.h>
#include <PrSDKThreadedWorkSuite.h>
#include "SDK_File.h"



#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <stdint.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #if defined(WINAPI_PARTITION_DESKTOP)
        #if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
            #define CCRIFT_MSW
        #else
            #error "Compile error: WINRT not supported"
        #endif
    #else
        #define CCRIFT_MSW
    #endif
#elif defined(linux) || defined(__linux) || defined(__linux__)
    #error "Compile error: Linux not supported"
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    #if TARGET_OS_IPHONE
        #error "Compile error: iOS not supported"
    #else
        #define CCRIFT_MAC
    #endif
    //#define __ASSERTMACROS__
#else
    #error "cinder compile error: Unknown platform"
#endif

#define PREVIEW_PLATFORM

#ifdef CCRIFT_MSW
#include "resource.h"
#endif

#ifdef CCRIFT_MSW
#define sleep(x) Sleep(x) 
#endif

#ifdef OCULUS_RIFT_PLATFORM
	#include "GL/CAPI_GLE.h"
	#include "Extras/OVR_Math.h"
	#include "Kernel/OVR_Log.h"
	#include "OVR_CAPI_GL.h"
#else
	#ifdef PREVIEW_PLATFORM
		#ifdef CCRIFT_MSW
		#define GLFW_EXPOSE_NATIVE_WIN32
		#define GLFW_EXPOSE_NATIVE_WGL
		#else
			#ifdef CCRIFT_MAC
				#define GLFW_EXPOSE_NATIVE_COCOA
				#define GLFW_EXPOSE_NATIVE_NSGL
			#endif
		#endif
        #if USE_GLAD
            #include <glad/glad.h>
        #else
            #include <GL/glew.h>
        #endif
		#include <GLFW/glfw3.h>
		#include <GLFW/glfw3native.h>
		#include <glm/fwd.hpp>
		#include <glm/glm.hpp>
		#include <glm/gtc/quaternion.hpp>
		#include <glm/gtc/matrix_transform.hpp>
		

        #define CCRIFT_OFFSETOF(class_, member_) offsetof(class_, member_)
	#endif

typedef struct ovrFovPort_
{
    float UpTan;
    float DownTan;
    float LeftTan;
    float RightTan;
} ovrFovPort;

#endif



#ifdef CCRIFT_MSW
    #define MAIN int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
#else
    #ifdef CCRIFT_MAC
        #define MAIN int main(void)
    #endif
#endif

#ifndef CCRIFT_MSW
#define HRESULT int32_t
#define S_OK 0
#define E_FAIL 2147500037
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif

#ifdef PRWIN_ENV
    #include <ctime>
#endif

#ifndef VALIDATE
    #define VALIDATE(x, msg) if (!(x)) { MessageBoxA(NULL, (msg), "CCRift Preview", MB_ICONERROR | MB_OK); exit(-1); }
#endif



#ifndef M_PI
    #define M_PI 3.14159265358979323846264338327f
#endif

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
	PrSDKWindowSuite*				WindowSuite;
} SDKSuites;

#ifdef CCRIFT_MSW
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
#endif

namespace CCRift
{
    // reference to uncompressed, simple image data. Does not manage memory
    typedef struct ImageData
    {
    public:
        ImageData() {}
        ImageData(int widthIn, int heightIn, int channelsIn, unsigned char *dataIn)
        : width(widthIn)
        , height(heightIn)
        , channels(channelsIn)
        , data(dataIn) {}
        
        virtual ~ImageData() {}
        
        int width;
        int height;
        int channels;
        unsigned char *data;
        
        // returns number of bytes allocated
        static size_t AllocEmptyImage(ImageData* img)
        {
            if(img == nullptr) return 0;
            size_t size = img->channels * img->width * img->height * sizeof(unsigned char);
            img->data = (unsigned char*)malloc(size);
            return size;
        }
        
        static void FreeImage(ImageData* img)
        {
            if(img == nullptr) return;
            free(img->data);
        }
    } ImageData;

	static std::string gWindowTitle = "Panorama Preview";

}

#define	PLUGIN_DISPLAY_NAME	L"Panorama Preview"


#endif //__CCRIFT_COMMONS_H__s