#include "CCRiftPlugin.h"
#include "CCRiftInstance.h"
//#include "CCRiftWinPreviewDevice.h"

#include "CCRiftGLFWPreviewDevice.h"

using namespace CCRift;

CCRiftPlugin::CCRiftPlugin(
	tmStdParms* ioStdParms,
	tmPluginInfo* outPluginInfo)
{
	// Here, you could make sure hardware is available

	

	copyConvertStringLiteralIntoUTF16(PLUGIN_DISPLAY_NAME, outPluginInfo->outDisplayName);

	outPluginInfo->outAudioAvailable = kPrFalse;
	outPluginInfo->outAudioDefaultEnabled = kPrFalse;
	outPluginInfo->outClockAvailable = kPrFalse;	// Set this to kPrFalse if the transmitter handles video only
	outPluginInfo->outVideoAvailable = kPrTrue;
	outPluginInfo->outVideoDefaultEnabled = kPrTrue;
	outPluginInfo->outHasSetup = kPrFalse; //kPrTrue


	// Acquire any suites needed!
	mSuites.SPBasic = ioStdParms->piSuites->utilFuncs->getSPBasicSuite();
	//mSuites.SPBasic->AcquireSuite(kPrSDKPlayModuleAudioSuite, kPrSDKPlayModuleAudioSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&mSuites.PlayModuleAudioSuite)));
	mSuites.SPBasic->AcquireSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&mSuites.PPixSuite)));
	mSuites.SPBasic->AcquireSuite(kPrSDKSequenceInfoSuite, kPrSDKSequenceInfoSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&mSuites.SequenceInfoSuite)));
	mSuites.SPBasic->AcquireSuite(kPrSDKThreadedWorkSuite, kPrSDKThreadedWorkSuiteVersion3, const_cast<const void**>(reinterpret_cast<void**>(&mSuites.ThreadedWorkSuite)));
	mSuites.SPBasic->AcquireSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&mSuites.TimeSuite)));

	mPreviewDevice = &IDevice<GLFWPreviewDevice>::Instance();

#ifdef CCRIFT_MSW
	mSuites.SPBasic->AcquireSuite(kPrSDKWindowSuite, kPrSDKWindowSuiteVersion, const_cast<const void**>(reinterpret_cast<void**>(&mSuites.WindowSuite)));
	prWnd wH = mSuites.WindowSuite->GetMainWindow();

	if(mPreviewDevice)
		mPreviewDevice->setMainWindowHandle(wH);
#endif
	//mPreviewDevice->start();
}

CCRiftPlugin::~CCRiftPlugin()
{
	// Be a good citizen and dispose of any suites used
	//mSuites.SPBasic->ReleaseSuite(kPrSDKPlayModuleAudioSuite, kPrSDKPlayModuleAudioSuiteVersion);
	mSuites.SPBasic->ReleaseSuite(kPrSDKPPixSuite, kPrSDKPPixSuiteVersion);
	mSuites.SPBasic->ReleaseSuite(kPrSDKSequenceInfoSuite, kPrSDKSequenceInfoSuiteVersion);
	mSuites.SPBasic->ReleaseSuite(kPrSDKThreadedWorkSuite, kPrSDKThreadedWorkSuiteVersion3);
	mSuites.SPBasic->ReleaseSuite(kPrSDKTimeSuite, kPrSDKTimeSuiteVersion);

	if (mPreviewDevice)
	{
		mPreviewDevice->stop();
	}

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
	/*if (mPreviewDevice && !mPreviewDevice->isRunning())
	{
		mPreviewDevice->start();
	}*/
	
	return tmResult_Success;
}

void* CCRiftPlugin::CreateInstance(
	const tmStdParms* inStdParms,
	tmInstance* inInstance)
{
#ifdef CCRIFT_MSW
	prWnd wH = mSuites.WindowSuite->GetMainWindow();

	if (mPreviewDevice)
		mPreviewDevice->setMainWindowHandle(wH);
#endif
	if (mPreviewDevice && !mPreviewDevice->isRunning())
	{
		mPreviewDevice->start();
	}
	return new CCRiftInstance(inInstance, mDevice, mSettings, mSuites, mPreviewDevice);
}

void CCRiftPlugin::DisposeInstance(
	const tmStdParms* inStdParms,
	tmInstance* inInstance)
{
	delete (CCRiftInstance*)inInstance->ioPrivateInstanceData;
	if (mPreviewDevice)
	{
		mPreviewDevice->stop();
	}
}