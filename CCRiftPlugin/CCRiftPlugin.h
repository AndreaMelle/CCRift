#ifndef __CCRIFT_PLUGIN_H__
#define __CCRIFT_PLUGIN_H__

#include "CCRiftCommons.h"

namespace CCRift
{
	class IDevice;
}

namespace CCRift
{
	/* The TransmitPlugin class is called by the TransmitModule to handle many calls from the transmit host.
	** It also handles creation and cleanup of TransmitInstances.
	** There could concievably be multiple plugins in a single module, although we only implement one here.
	*/
	class CCRiftPlugin
	{
	public:
		CCRiftPlugin(
			tmStdParms* ioStdParms,
			tmPluginInfo* outPluginInfo);

		virtual ~CCRiftPlugin();

		tmResult SetupDialog(
			tmStdParms* ioStdParms,
			prParentWnd inParentWnd);

		tmResult NeedsReset(
			const tmStdParms* inStdParms,
			prBool* outResetModule);

		void* CreateInstance(
			const tmStdParms* inStdParms,
			tmInstance* inInstance);

		void DisposeInstance(
			const tmStdParms* inStdParms,
			tmInstance* inInstance);

	private:
		SDKDevicePtr	mDevice;
		SDKSettings		mSettings;
		SDKSuites		mSuites;

		IDevice *mPreviewDevice;
	};
}

#endif //__CCRIFT_PLUGIN_H__