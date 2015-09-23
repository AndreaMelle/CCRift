#ifndef __CCRIFT_INSTANCE_H__
#define __CCRIFT_INSTANCE_H__

#include "CCRiftCommons.h"

namespace CCRift
{
	class WinPreviewDevice;
}

namespace CCRift
{
	/* The TransmitInstance class is called by the TransmitModule and the TransmitPlugin to handle many calls from the transmit host.
	** There can be several instances outstanding at any given time.
	*/
	class CCRiftInstance
	{
	public:
		CCRiftInstance(
			const tmInstance* inInstance,
			const SDKDevicePtr& inDevice,
			const SDKSettings& inSettings,
			const SDKSuites& inSuites,
			WinPreviewDevice* previewDevice);

		~CCRiftInstance();

		tmResult QueryVideoMode(
			const tmStdParms* inStdParms,
			const tmInstance* inInstance,
			csSDK_int32 inQueryIterationIndex,
			tmVideoMode* outVideoMode);

		tmResult ActivateDeactivate(
			const tmStdParms* inStdParms,
			const tmInstance* inInstance,
			PrActivationEvent inActivationEvent,
			prBool inAudioActive,
			prBool inVideoActive);

		tmResult StartPlaybackClock(
			const tmStdParms* inStdParms,
			const tmInstance* inInstance,
			const tmPlaybackClock* inClock);

		tmResult StopPlaybackClock(
			const tmStdParms* inStdParms,
			const tmInstance* inInstance);

		tmResult PushVideo(
			const tmStdParms* inStdParms,
			const tmInstance* inInstance,
			const tmPushVideo* inPushVideo);

	private:
		// These members immediately below (before the empty line) all get initialized by being passed in from the TransmitPlugin
		SDKDevicePtr				mDevice;
		SDKSettings					mSettings;
		SDKSuites					mSuites;

		PrTime						mTicksPerSecond;
		PrTime						mVideoFrameRate;

		tmClockCallback				mClockCallback;
		void *						mCallbackContext;
		ThreadedWorkRegistration	mUpdateClockRegistration;

		float						mPlaybackSpeed;
		prBool						mPlaying;

		WinPreviewDevice* mPreviewDevice;
	};
}

#endif //__CCRIFT_INSTANCE_H__