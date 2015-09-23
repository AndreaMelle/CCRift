#include "CCRiftInstance.h"
#include <stdio.h>
#include <ctime>
#include "CCRiftScene.h"
#include <Kernel/OVR_System.h>
#include "OVR_CAPI_GL.h"
#include "CCRiftCommons.h"
#include "CCRiftWinPreviewDevice.h"

using namespace OVR;
using namespace CCRift;


struct ClockInstanceData {
	PrTime						startTime;
	PrTime						ticksPerSecond;
	PrTime						videoFrameRate;
	tmClockCallback				clockCallback;
	void **						callbackContextPtr;
	PrPlayID					playID;
	float						audioSampleRate;
	float **					audioBuffers;
	SDKSuites					suites;
};

/* This plug-in defined function is called on a new thread when StartPlaybackClock is called.
** It loops continuously, calling the tmClockCallback at regular intervals until playback ends.
** We try to make a call at same frequency as the frame rate of the timeline (i.e. transmit instance)
** TRICKY: How does the function know when playback ends and it should end the loop?
** Answer: The ClockInstanceData passed in contains a pointer to the callbackContext.
** When playback ends, the context is set to zero, and that's how it knows to end the loop. 
*/
void UpdateClock(
	void* inInstanceData,
	csSDK_int32 inPluginID,
	prSuiteError inStatus)
{
	ClockInstanceData	*clockInstanceData	= 0;
	clock_t				latestClockTime = clock();
	PrTime				timeElapsed = 0;
	int					audioSampleCount = 0;
		
	clockInstanceData = reinterpret_cast<ClockInstanceData*>(inInstanceData);

	// Calculate how long to wait in between clock updates
	clock_t timeBetweenClockUpdates = (clock_t)(clockInstanceData->videoFrameRate * CLOCKS_PER_SEC / clockInstanceData->ticksPerSecond);

	#ifdef PRWIN_ENV		
	char outputString[255];
	sprintf_s(	outputString, 255,
				"<%i> New clock started with callback context 0x%llx.\n",
				clock(),
				*clockInstanceData->callbackContextPtr);
	OutputDebugString(outputString);
	#elif defined PRMAC_ENV
	NSLog(		@"New clock started with callback context 0x%llx.",
				(long long)*clockInstanceData->callbackContextPtr);
	#endif

	// Loop as long as we have a valid clock callback.
	// It will be set to NULL when playback stops and this function can return.
	while (clockInstanceData->clockCallback && *clockInstanceData->callbackContextPtr)
	{
		// Calculate time elapsed since last time we checked the clock
		clock_t newTime = clock();
		clock_t tempTimeElapsed = newTime - latestClockTime;
		latestClockTime = newTime;

		// Convert tempTimeElapsed to PrTime
		timeElapsed = tempTimeElapsed * clockInstanceData->ticksPerSecond / CLOCKS_PER_SEC;

		#ifdef PRWIN_ENV
		char outputString[255];
		sprintf_s(	outputString, 255,
					"<%i> Clock callback made. %llu ticks elapsed.\n",
					clock(),
					timeElapsed);
		OutputDebugString(outputString);
		#elif defined PRMAC_ENV
		NSLog(		@"Clock callback made. %llu ticks, or %lu microseconds elapsed. Sleeping for %lu microseconds.",
					timeElapsed,
					tempTimeElapsed,
					timeBetweenClockUpdates);
		#endif

		clockInstanceData->clockCallback(*clockInstanceData->callbackContextPtr, timeElapsed);

		// Sleep for a frame's length
		#ifdef PRWIN_ENV
		Sleep(timeBetweenClockUpdates);
		#elif defined PRMAC_ENV
		usleep(timeBetweenClockUpdates / 2); // Try sleeping for the half the time, since Mac OS seems to oversleep :)
		#endif
	}

	#ifdef PRWIN_ENV
	outputString[255];
	sprintf_s(	outputString, 255,
				"<%i> Clock with callback context %llx exited.\n",
				clock(),
				*clockInstanceData->callbackContextPtr);
	OutputDebugString(outputString);
	#elif defined PRMAC_ENV
	NSLog(		@"Clock with callback context %llx exited.",
				(long long)*clockInstanceData->callbackContextPtr);
	#endif

	delete(clockInstanceData);
}

CCRiftInstance::CCRiftInstance(const tmInstance* inInstance,
	const SDKDevicePtr& inDevice,
	const SDKSettings& inSettings,
	const SDKSuites& inSuites,
	WinPreviewDevice* previewDevice)
	: mDevice(inDevice)
	, mSettings(inSettings)
	, mSuites(inSuites)
	, mPreviewDevice(previewDevice)
{
	mClockCallback = 0;
	mCallbackContext = 0;
	mUpdateClockRegistration = 0;
	mPlaying = kPrFalse;

	mSuites.TimeSuite->GetTicksPerSecond(&mTicksPerSecond);

#ifdef PRWIN_ENV
	char outputString[255];

	sprintf_s(outputString, 255,
		"Clocks per second: %i.\n",
		CLOCKS_PER_SEC);
	OutputDebugString(outputString);
#elif defined PRMAC_ENV
	NSLog(@"Clocks per second: %i.",
		CLOCKS_PER_SEC);
#endif
}


CCRiftInstance::~CCRiftInstance()
{
	
}

tmResult CCRiftInstance::QueryVideoMode(const tmStdParms* inStdParms,
	const tmInstance* inInstance,
	csSDK_int32 inQueryIterationIndex,
	tmVideoMode* outVideoMode)
{
	tmResult returnVal = tmResult_Success;

	outVideoMode->outWidth = 1920;
	outVideoMode->outHeight = 960;
	outVideoMode->outPARNum = 1;
	outVideoMode->outPARDen = 1;
	outVideoMode->outFieldType = prFieldsNone;
	outVideoMode->outPixelFormat = PrPixelFormat_BGRA_4444_8u;
	//outVideoMode->outLatency = inInstance->inVideoFrameRate * 5; // Ask for 5 frames preroll
	mVideoFrameRate = inInstance->inVideoFrameRate;
	return returnVal;
}

tmResult CCRiftInstance::ActivateDeactivate(const tmStdParms* inStdParms,
	const tmInstance* inInstance,
	PrActivationEvent inActivationEvent,
	prBool inAudioActive,
	prBool inVideoActive)
{
	#ifdef PRWIN_ENV
	char outputString[255];
	sprintf_s(	outputString, 255,
				"<%i> ActivateDeactivate called ",
				clock());
	OutputDebugString(outputString);
	#elif defined PRMAC_ENV
	NSLog(@"ActivateDeactivate called.");
	#endif

	if (inAudioActive || inVideoActive)
	{
	//	mDevice->StartTransmit();
		#ifdef PRWIN_ENV
		if (inAudioActive && inVideoActive)
			OutputDebugString("with audio active and video active.\n");
		else if (inAudioActive)
			OutputDebugString("with audio active.\n");
		else
			OutputDebugString("with video active.\n");
		#elif defined PRMAC_ENV
		if (inAudioActive && inVideoActive)
			NSLog(@"with audio active and video active.");
		else if (inAudioActive)
			NSLog(@"with audio active.");
		else
			NSLog(@"with video active.");			
		#endif
	}
	else
	{
	//	mDevice->StopTransmit();
		#ifdef PRWIN_ENV
		OutputDebugString("to deactivate.\n");
		#elif defined PRMAC_ENV
		NSLog(@"to deactivate.");
		#endif
	}

	return tmResult_Success;
}
	
tmResult CCRiftInstance::StartPlaybackClock(const tmStdParms* inStdParms,
	const tmInstance* inInstance,
	const tmPlaybackClock* inClock)
{
	float			frameTimeInSeconds	= 0;

	mClockCallback				= inClock->inClockCallback;
	mCallbackContext			= inClock->inCallbackContext;
	mPlaybackSpeed				= inClock->inSpeed;
	mUpdateClockRegistration	= 0;

	frameTimeInSeconds = (float) inClock->inStartTime / mTicksPerSecond;

	#ifdef PRWIN_ENV
	char outputString[255];
	sprintf_s(	outputString, 255,
				"<%i> StartPlaybackClock called for time %7.2f.",
				clock(),
				frameTimeInSeconds);
	OutputDebugString(outputString);

	if (inClock->inPlayMode == playmode_Scrubbing)
	{
		sprintf_s(	outputString, 255,
					" Scrubbing.\n");
	}
	else if (inClock->inPlayMode == playmode_Playing)
	{
		sprintf_s(	outputString, 255,
					" Playing.\n");
	}

	OutputDebugString(outputString);
	#elif defined PRMAC_ENV
	if (inClock->inPlayMode == playmode_Scrubbing)
	{
		NSLog(@"StartPlaybackClock called for time %7.2f. Scrubbing.", frameTimeInSeconds);
	}
	else if (inClock->inPlayMode == playmode_Playing)
	{
		NSLog(@"StartPlaybackClock called for time %7.2f. Playing.", frameTimeInSeconds);
	}
	#endif

	// If not yet playing, and called to play,
	// then register our UpdateClock function that calls the audio callback asynchronously during playback
	// Note that StartPlaybackClock can be called multiple times without a StopPlaybackClock,
	// for example if changing playback speed in the timeline.
	// If already playing, we the callbackContext doesn't change, and we let the current clock continue.
	if (!mPlaying && inClock->inPlayMode == playmode_Playing)
	{
		mPlaying = kPrTrue;

		// Initialize the ClockInstanceData that the UpdateClock function will need
		// We allocate the data here, and the data will be disposed at the end of the UpdateClock function
		ClockInstanceData *instanceData = new ClockInstanceData;
		instanceData->startTime = inClock->inStartTime;
		instanceData->callbackContextPtr = &mCallbackContext;
		instanceData->clockCallback = mClockCallback;
		instanceData->ticksPerSecond = mTicksPerSecond;
		instanceData->videoFrameRate = mVideoFrameRate;
		instanceData->playID = inInstance->inPlayID;
		instanceData->suites = mSuites;

		// Cross-platform threading suites!
		mSuites.ThreadedWorkSuite->RegisterForThreadedWork(	&UpdateClock,
															instanceData,
															&mUpdateClockRegistration);
		mSuites.ThreadedWorkSuite->QueueThreadedWork(mUpdateClockRegistration, inInstance->inInstanceID);
	}

	//SetForegroundWindow()

	return tmResult_Success;
}

tmResult CCRiftInstance::StopPlaybackClock(const tmStdParms* inStdParms,
const tmInstance* inInstance)
{
	mClockCallback = 0;
	mCallbackContext = 0;
	mPlaying = kPrFalse;

	if (mUpdateClockRegistration)
	{
		mSuites.ThreadedWorkSuite->UnregisterForThreadedWork(mUpdateClockRegistration);
		mUpdateClockRegistration = 0;
	}

	#ifdef PRWIN_ENV
	char outputString[255];
	sprintf_s(	outputString, 255,
				"<%i> StopPlaybackClock called.\n",
				clock());
	OutputDebugString(outputString);
	#elif defined PRMAC_ENV
	NSLog(@"StopPlaybackClock called.");
	#endif

	return tmResult_Success;
}

tmResult CCRiftInstance::PushVideo(const tmStdParms* inStdParms,
	const tmInstance* inInstance,
	const tmPushVideo* inPushVideo)
{
	// Send the video frames to the hardware.  We also log frame info to the debug console.
	float			frameTimeInSeconds	= 0;
	prRect			frameBounds;
	csSDK_uint32	parNum				= 0,
					parDen				= 0;
	PrPixelFormat	pixelFormat			= PrPixelFormat_Invalid;
	int				audioSampleCount	= 0;
	PrTime			zeroPointTime		= 0;
	prBool			dropFrame			= kPrFalse;
	prSuiteError	returnValue			= 0;

	frameTimeInSeconds = (float) inPushVideo->inTime / mTicksPerSecond;
	mSuites.PPixSuite->GetBounds(inPushVideo->inFrames[0].inFrame, &frameBounds);
	mSuites.PPixSuite->GetPixelAspectRatio(inPushVideo->inFrames[0].inFrame, &parNum, &parDen);
	mSuites.PPixSuite->GetPixelFormat(inPushVideo->inFrames[0].inFrame, &pixelFormat);

	//mSuites.

	mSuites.SequenceInfoSuite->GetZeroPoint(inInstance->inTimelineID, &zeroPointTime);
	mSuites.SequenceInfoSuite->GetTimecodeDropFrame(inInstance->inTimelineID, &dropFrame);

	#ifdef PRWIN_ENV
		
	char outputString[255];
	sprintf_s(	outputString, 255,
				"<%i> PushVideo called for time %7.2f, frame size: %d x %d, PAR: %4.3f, pixel format: %#x. %d",
				clock(),
				frameTimeInSeconds,
				abs(frameBounds.right - frameBounds.left),
				abs(frameBounds.top - frameBounds.bottom),
				(float) parNum / parDen,
				pixelFormat,
				inPushVideo->inFrameCount);
	OutputDebugString(outputString);

	if (inPushVideo->inPlayMode == playmode_Scrubbing)
	{
		sprintf_s(	outputString, 255,
					" Scrubbing.\n");
	}
	else if (inPushVideo->inPlayMode == playmode_Playing)
	{
		sprintf_s(	outputString, 255,
					" Playing.\n");
	}
	else if (inPushVideo->inPlayMode == playmode_Stopped)
	{
		sprintf_s(	outputString, 255,
					" Stopped.\n");
	}
	OutputDebugString(outputString);

	#elif defined PRMAC_ENV
		
	NSLog(@"PushVideo called for time %7.2f, frame size: %d x %d, PAR: %4.3f, pixel format: %#x.",
			frameTimeInSeconds,
			abs(frameBounds.right - frameBounds.left),
			abs(frameBounds.top - frameBounds.bottom),
			(float) parNum / parDen,
			pixelFormat);
		
	#endif

	//
	// This is where a transmit plug-in could queue up the frame to an actual hardware device.
	//

	LONG w = abs(frameBounds.right - frameBounds.left);
	LONG h = abs(frameBounds.top - frameBounds.bottom);
		
	if (mPreviewDevice && mPreviewDevice->isRunning())
	{

		if (pixelFormat == PrPixelFormat_BGRA_4444_8u
			&& w == mPreviewDevice->preferredFrameWidth() && h == mPreviewDevice->preferredFrameHeight())
		{
			char *data;

			for (int i = 0; i < inPushVideo->inFrameCount; i++)
			{
				mSuites.PPixSuite->GetPixels(inPushVideo->inFrames[i].inFrame, PrPPixBufferAccess_ReadOnly, &data);
				break;
			}

			mPreviewDevice->pushFrame((unsigned char*)data);
		}

			
	}
		

	// Dispose of the PPix(es) when done!
	for (int i=0; i< inPushVideo->inFrameCount; i++)
	{
		mSuites.PPixSuite->Dispose(inPushVideo->inFrames[i].inFrame);
	}

	return tmResult_Success;
}