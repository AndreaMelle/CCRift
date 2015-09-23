#include "CCRiftPlugin.h"
#include "CCRiftInstance.h"
#include "CCRiftWinPreviewDevice.h"

using namespace CCRift;

CCRiftPlugin::CCRiftPlugin(
	tmStdParms* ioStdParms,
	tmPluginInfo* outPluginInfo)
{
	// Here, you could make sure hardware is available

	mPreviewDevice = new WinPreviewDevice();
	mPreviewDevice->start();

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

CCRiftPlugin::~CCRiftPlugin()
{
	// Be a good citizen and dispose of any suites used
	mSuites.SPBasic->ReleaseSuite(kPrSDKPlayModuleAudioSuite, kPrSDKPlayModuleAudioSuiteVersion);
	mSuites.SPBasic->ReleaseSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion);
	mSuites.SPBasic->ReleaseSuite(kPrSDKSequenceInfoSuite, kPrSDKSequenceInfoSuiteVersion);
	mSuites.SPBasic->ReleaseSuite(kPrSDKThreadedWorkSuite, kPrSDKThreadedWorkSuiteVersion3);
	mSuites.SPBasic->ReleaseSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion);

	mPreviewDevice->stop();
	delete mPreviewDevice;

}

tmResult CCRiftPlugin::SetupDialog(
	tmStdParms* ioStdParms,
	prParentWnd inParentWnd)
{
	// Get the settings, display a modal setup dialog for the user
	// MessageBox()

	// If the user changed the settings, save the new settings back to
	// ioStdParms->ioSerializedPluginData, and update ioStdParms->ioSerializedPluginDataSize

	return tmResult_Success;
}

tmResult CCRiftPlugin::NeedsReset(
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

void* CCRiftPlugin::CreateInstance(
	const tmStdParms* inStdParms,
	tmInstance* inInstance)
{

	return new CCRiftInstance(inInstance, mDevice, mSettings, mSuites, mPreviewDevice);
}

void CCRiftPlugin::DisposeInstance(
	const tmStdParms* inStdParms,
	tmInstance* inInstance)
{
	delete (CCRiftInstance*)inInstance->ioPrivateInstanceData;
}