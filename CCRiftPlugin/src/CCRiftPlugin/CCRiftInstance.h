#ifndef __CCRIFT_INSTANCE_H__
#define __CCRIFT_INSTANCE_H__

#include "CCRiftCommons.h"

namespace CCRift
{
	class GLFWPreviewDevice;
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
			GLFWPreviewDevice* previewDevice);

		virtual ~CCRiftInstance();

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

		tmResult PushVideo(
			const tmStdParms* inStdParms,
			const tmInstance* inInstance,
			const tmPushVideo* inPushVideo);

	private:
		SDKDevicePtr				mDevice;
		SDKSettings					mSettings;
		SDKSuites					mSuites;

		PrTime mTicksPerSecond;
		PrTime mVideoFrameRate;


		float						mPlaybackSpeed;
		prBool						mPlaying;
		prBool						mSelfActive;

		GLFWPreviewDevice* mPreviewDevice;
	};
}

#endif //__CCRIFT_INSTANCE_H__