#include "CCRiftPlugin.h"
#include "CCRiftInstance.h"

using namespace CCRift;

/* The TransmitModule class is the main class.  Function pointers to its static methods are
** provided directly to the transmit host.  The TransmitModule, in turn, makes calls
** into the TransmitPlugin or TransmitInstance, as appropriate.
** A pointer to the TransmitPlugin is saved in ioStdParms->ioPrivatePluginData, and
** pointers to TransmitInstances are saved in ioPrivateInstanceData.
*/
class TransmitModule
{
public:
	static tmResult Startup(
		tmStdParms* ioStdParms,
		tmPluginInfo* outPluginInfo)
	{
		ioStdParms->ioPrivatePluginData = new CCRiftPlugin(ioStdParms, outPluginInfo);
		return tmResult_Success;
	}

	static tmResult Shutdown(
		tmStdParms* ioStdParms)
	{
		delete (CCRiftPlugin*)ioStdParms->ioPrivatePluginData;
		ioStdParms->ioPrivatePluginData = 0;
		return tmResult_Success;
	}

	static tmResult SetupDialog(
		tmStdParms* ioStdParms,
		prParentWnd inParentWnd)
	{
		return ((CCRiftPlugin*)ioStdParms->ioPrivatePluginData)->SetupDialog(ioStdParms, inParentWnd);
	}

	static tmResult NeedsReset(
		const tmStdParms* inStdParms,
		prBool* outResetModule)
	{
		return ((CCRiftPlugin*)inStdParms->ioPrivatePluginData)->NeedsReset(inStdParms, outResetModule);
	}

	static tmResult CreateInstance(
		const tmStdParms* inStdParms,
		tmInstance* ioInstance)
	{
		ioInstance->ioPrivateInstanceData = ((CCRiftPlugin*)inStdParms->ioPrivatePluginData)->CreateInstance(inStdParms, ioInstance);
		return ioInstance->ioPrivateInstanceData != 0 ? tmResult_Success : tmResult_ErrorUnknown;
	}

	static tmResult DisposeInstance(
		const tmStdParms* inStdParms,
		tmInstance* ioInstance)
	{
		((CCRiftPlugin*)inStdParms->ioPrivatePluginData)->DisposeInstance(inStdParms, ioInstance);
		ioInstance->ioPrivateInstanceData = 0;
		return tmResult_Success;
	}

	static tmResult QueryAudioMode(
		const tmStdParms* inStdParms,
		const tmInstance* inInstance,
		csSDK_int32 inQueryIterationIndex,
		tmAudioMode* outAudioMode)
	{
		return tmResult_Success;// ((TransmitInstance*)inInstance->ioPrivateInstanceData)->QueryAudioMode(inStdParms, inInstance, inQueryIterationIndex, outAudioMode);
	}

	static tmResult QueryVideoMode(
		const tmStdParms* inStdParms,
		const tmInstance* inInstance,
		csSDK_int32 inQueryIterationIndex,
		tmVideoMode* outVideoMode)
	{
		return ((CCRiftInstance*)inInstance->ioPrivateInstanceData)->QueryVideoMode(inStdParms, inInstance, inQueryIterationIndex, outVideoMode);
	}

	static tmResult ActivateDeactivate(
		const tmStdParms* inStdParms,
		const tmInstance* inInstance,
		PrActivationEvent inActivationEvent,
		prBool inAudioActive,
		prBool inVideoActive)
	{
		return ((CCRiftInstance*)inInstance->ioPrivateInstanceData)->ActivateDeactivate(inStdParms, inInstance, inActivationEvent, inAudioActive, inVideoActive);
	}

	static tmResult StartPlaybackClock(
		const tmStdParms* inStdParms,
		const tmInstance* inInstance,
		const tmPlaybackClock* inClock)
	{
		return ((CCRiftInstance*)inInstance->ioPrivateInstanceData)->StartPlaybackClock(inStdParms, inInstance, inClock);
	}

	static tmResult StopPlaybackClock(
		const tmStdParms* inStdParms,
		const tmInstance* inInstance)
	{
		return ((CCRiftInstance*)inInstance->ioPrivateInstanceData)->StopPlaybackClock(inStdParms, inInstance);
	}

	static tmResult PushVideo(
		const tmStdParms* inStdParms,
		const tmInstance* inInstance,
		const tmPushVideo* inPushVideo)
	{
		return ((CCRiftInstance*)inInstance->ioPrivateInstanceData)->PushVideo(inStdParms, inInstance, inPushVideo);
	}
};


extern "C" {
	DllExport PREMPLUGENTRY xTransmitEntry(
		csSDK_int32	inInterfaceVersion,
		prBool		inLoadModule,
		piSuitesPtr	piSuites,
		tmModule*	outModule)
	{
		tmResult result = tmResult_Success;

		if (inLoadModule)
		{
			outModule->Startup = TransmitModule::Startup;
			outModule->Shutdown = TransmitModule::Shutdown;
			outModule->SetupDialog = TransmitModule::SetupDialog;
			outModule->NeedsReset = TransmitModule::NeedsReset;
			outModule->CreateInstance = TransmitModule::CreateInstance;
			outModule->QueryAudioMode = TransmitModule::QueryAudioMode;
			outModule->QueryVideoMode = TransmitModule::QueryVideoMode;
			outModule->ActivateDeactivate = TransmitModule::ActivateDeactivate;
			outModule->StartPlaybackClock = TransmitModule::StartPlaybackClock;
			outModule->StopPlaybackClock = TransmitModule::StopPlaybackClock;
			outModule->PushVideo = TransmitModule::PushVideo;
		}
		else
		{
			// The module is being unloaded. Nothing to do here in our implementation.
		}
		return result;
	}
} // extern "C"
