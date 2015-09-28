#include "CCRiftInstance.h"
#include "CCRiftCommons.h"
#include "CCRiftScene.h"
#include "CCRiftGLFWPreviewDevice.h"

using namespace CCRift;

CCRiftInstance::CCRiftInstance(const tmInstance* inInstance,
	const SDKDevicePtr& inDevice,
	const SDKSettings& inSettings,
	const SDKSuites& inSuites,
	GLFWPreviewDevice* previewDevice)
	: mDevice(inDevice)
	, mSettings(inSettings)
	, mSuites(inSuites)
	, mPreviewDevice(previewDevice)
	, mSelfActive(false)
{
	mPlaying = kPrFalse;
	mSuites.TimeSuite->GetTicksPerSecond(&mTicksPerSecond);
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

	outVideoMode->outWidth = mPreviewDevice->preferredFrameWidth();
	outVideoMode->outHeight = mPreviewDevice->preferredFrameHeight();
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

	if (inVideoActive)
	{
		//	mDevice->StartTransmit();
		mPreviewDevice->setActive(true);
		mSelfActive = true;
	}
	else
	{
		//	mDevice->StopTransmit();
		mPreviewDevice->setActive(false);
		mSelfActive = false;
	}

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
		
	if (mSelfActive && mPreviewDevice && mPreviewDevice->isRunning())
	{
		if (pixelFormat == PrPixelFormat_BGRA_4444_8u
			&& w == mPreviewDevice->preferredFrameWidth()
			&& h == mPreviewDevice->preferredFrameHeight())
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