/*******************************************************************/
/*                                                                 */
/*                      ADOBE CONFIDENTIAL                         */
/*                   _ _ _ _ _ _ _ _ _ _ _ _ _                     */
/*                                                                 */
/* Copyright 2012 Adobe Systems Incorporated					   */
/* All Rights Reserved.                                            */
/*                                                                 */
/* NOTICE:  All information contained herein is, and remains the   */
/* property of Adobe Systems Incorporated and its suppliers, if    */
/* any.  The intellectual and technical concepts contained         */
/* herein are proprietary to Adobe Systems Incorporated and its    */
/* suppliers and may be covered by U.S. and Foreign Patents,       */
/* patents in process, and are protected by trade secret or        */
/* copyright law.  Dissemination of this information or            */
/* reproduction of this material is strictly forbidden unless      */
/* prior written permission is obtained from Adobe Systems         */
/* Incorporated.                                                   */
/*                                                                 */
/*******************************************************************/


#include "TransmitterPlugin.h"
#include <stdio.h>
#include <ctime>
#include "CCRiftScene.h"
#include <Kernel/OVR_System.h>
#include "OVR_CAPI_GL.h"
#include "CCRiftCommons.h"

using namespace OVR;
using namespace SDK;
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

			// How many audio samples shall we request?  Calculate the number of audio samples in one frame
			audioSampleCount = (int) (clockInstanceData->audioSampleRate * clockInstanceData->videoFrameRate / clockInstanceData->ticksPerSecond);
			if (audioSampleCount > AUDIO_BUFFER_SIZE)
			{
				// If we get here, we underestimated the size of the audio buffer, and may need to adjust it higher.
				#ifdef PRWIN_ENV
				OutputDebugString("AUDIO_BUFFER_SIZE too small.\n");
				#elif defined PRMAC_ENV
				NSLog(@"AUDIO_BUFFER_SIZE too small.");
				#endif

				audioSampleCount = AUDIO_BUFFER_SIZE;
			}

			// Request the audio!  But... we don't do anything with it for now.  At least we know if it's valid.
			clockInstanceData->suites.PlayModuleAudioSuite->GetNextAudioBuffer(clockInstanceData->playID, 0, clockInstanceData->audioBuffers, audioSampleCount);

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


////
//// TransmitInstance methods
////

	/*
	**
	*/
	TransmitInstance::TransmitInstance(
		const tmInstance* inInstance,
		const SDKDevicePtr& inDevice,
		const SDKSettings& inSettings,
		const SDKSuites& inSuites)
		:
		mDevice(inDevice),
		mSettings(inSettings),
		mSuites(inSuites)
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

		context = new OGLPlatform();

		ovrGraphicsLuid luid;
		ovrSizei windowSize = { 1920 / 2, 1080 / 2 };

		VALIDATE(context->InitWindow(NULL, L"Preview"), "Failed to open window.");

		if (!context->InitDevice(windowSize.w, windowSize.h, reinterpret_cast<LUID*>(&luid)))
			return;

		roomScene = nullptr;
		roomScene = new Scene();

		roomScene->Init(*context, windowSize);
	}

	/* Shutdown is handled here.
	**
	*/
	TransmitInstance::~TransmitInstance()
	{
		roomScene->Release(*context);
		delete roomScene;
		context->ReleaseDevice();
		delete context;
	}

	/* We're not picky.  We claim to support any format the host can throw at us (yeah right).
	**
	*/
	tmResult TransmitInstance::QueryVideoMode(
		const tmStdParms* inStdParms,
		const tmInstance* inInstance,
		csSDK_int32 inQueryIterationIndex,
		tmVideoMode* outVideoMode)
	{
		tmResult returnVal = tmResult_Success;

		outVideoMode->outWidth = 1920;
		outVideoMode->outHeight = 960;
		outVideoMode->outPARNum = 0;
		outVideoMode->outPARDen = 0;
		outVideoMode->outFieldType = prFieldsNone;
		outVideoMode->outPixelFormat = PrPixelFormat_BGRA_4444_8u;
		outVideoMode->outLatency = inInstance->inVideoFrameRate * 5; // Ask for 5 frames preroll

		mVideoFrameRate = inInstance->inVideoFrameRate;

		return returnVal;
	}

	/*
	**
	*/
	tmResult TransmitInstance::ActivateDeactivate(
		const tmStdParms* inStdParms,
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
	
	/*
	**
	*/
	tmResult TransmitInstance::StartPlaybackClock(
		const tmStdParms* inStdParms,
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

		return tmResult_Success;
	}

	/*
	**
	*/
	tmResult TransmitInstance::StopPlaybackClock(
		const tmStdParms* inStdParms,
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

	/*
	**
	*/
	tmResult TransmitInstance::PushVideo(
		const tmStdParms* inStdParms,
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

		mSuites.SequenceInfoSuite->GetZeroPoint(inInstance->inTimelineID, &zeroPointTime);
		mSuites.SequenceInfoSuite->GetTimecodeDropFrame(inInstance->inTimelineID, &dropFrame);

		#ifdef PRWIN_ENV
		
		char outputString[255];
		sprintf_s(	outputString, 255,
					"<%i> PushVideo called for time %7.2f, frame size: %d x %d, PAR: %4.3f, pixel format: %#x.",
					clock(),
					frameTimeInSeconds,
					abs(frameBounds.right - frameBounds.left),
					abs(frameBounds.top - frameBounds.bottom),
					(float) parNum / parDen,
					pixelFormat);
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
		if(context->HandleMessages())
		{
			roomScene->Loop(*context);
		}
		

		// Dispose of the PPix(es) when done!
		for (int i=0; i< inPushVideo->inFrameCount; i++)
		{
			mSuites.PPixSuite->Dispose(inPushVideo->inFrames[i].inFrame);
		}

		return tmResult_Success;
	}


////
//// TransmitPlugin methods
////

	/* Startup is handled here.
	**
	*/
	TransmitPlugin::TransmitPlugin(
		tmStdParms* ioStdParms,
		tmPluginInfo* outPluginInfo)
	{
		// Here, you could make sure hardware is available
		copyConvertStringLiteralIntoUTF16(PLUGIN_DISPLAY_NAME, outPluginInfo->outDisplayName);
		
		outPluginInfo->outAudioAvailable = kPrTrue;
		outPluginInfo->outAudioDefaultEnabled = kPrFalse;
		outPluginInfo->outClockAvailable = kPrTrue;	// Set this to kPrFalse if the transmitter handles video only
		outPluginInfo->outVideoAvailable = kPrTrue;
		outPluginInfo->outVideoDefaultEnabled = kPrTrue;
		outPluginInfo->outHasSetup = kPrTrue;

		// Acquire any suites needed!
		mSuites.SPBasic = ioStdParms->piSuites->utilFuncs->getSPBasicSuite();
		mSuites.SPBasic->AcquireSuite(kPrSDKPlayModuleAudioSuite, kPrSDKPlayModuleAudioSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&mSuites.PlayModuleAudioSuite)));
		mSuites.SPBasic->AcquireSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&mSuites.PPixSuite)));
		mSuites.SPBasic->AcquireSuite(kPrSDKSequenceInfoSuite, kPrSDKSequenceInfoSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&mSuites.SequenceInfoSuite)));
		mSuites.SPBasic->AcquireSuite(kPrSDKThreadedWorkSuite, kPrSDKThreadedWorkSuiteVersion3, const_cast<const void**>(reinterpret_cast<void**>(&mSuites.ThreadedWorkSuite)));
		mSuites.SPBasic->AcquireSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&mSuites.TimeSuite)));
	}

	/* Shutdown is handled here.
	**
	*/
	TransmitPlugin::~TransmitPlugin()
	{
		// Be a good citizen and dispose of any suites used
		mSuites.SPBasic->ReleaseSuite(kPrSDKPlayModuleAudioSuite, kPrSDKPlayModuleAudioSuiteVersion);
		mSuites.SPBasic->ReleaseSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion);
		mSuites.SPBasic->ReleaseSuite(kPrSDKSequenceInfoSuite, kPrSDKSequenceInfoSuiteVersion);
		mSuites.SPBasic->ReleaseSuite(kPrSDKThreadedWorkSuite, kPrSDKThreadedWorkSuiteVersion3);
		mSuites.SPBasic->ReleaseSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion);
	}
	
	/*
	**
	*/
	tmResult TransmitPlugin::SetupDialog(
		tmStdParms* ioStdParms,
		prParentWnd inParentWnd)
	{
		// Get the settings, display a modal setup dialog for the user
		// MessageBox()

		// If the user changed the settings, save the new settings back to
		// ioStdParms->ioSerializedPluginData, and update ioStdParms->ioSerializedPluginDataSize

		return tmResult_Success;
	}
	
	/*
	**
	*/
	tmResult TransmitPlugin::NeedsReset(
		const tmStdParms* inStdParms,
		prBool* outResetModule)
	{
		// Did the hardware change?
		// if (it did)
		//{
		//	*outResetModule = kPrTrue;
		//}
		return tmResult_Success;
	}
	
	/*
	**
	*/
	void* TransmitPlugin::CreateInstance(
		const tmStdParms* inStdParms,
		tmInstance* inInstance)
	{
		
		return new TransmitInstance(inInstance, mDevice, mSettings, mSuites);
	}

	void TransmitPlugin::DisposeInstance(
		const tmStdParms* inStdParms,
		tmInstance* inInstance)
	{
		delete (TransmitInstance*)inInstance->ioPrivateInstanceData;
	}
