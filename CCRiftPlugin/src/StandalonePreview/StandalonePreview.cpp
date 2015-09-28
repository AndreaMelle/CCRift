#include "CCRiftCommons.h"

#include "CCRiftScene.h"
#include "CCRiftWinPreviewDevice.h"
#include "CCRiftGLFWPreviewDevice.h"

using namespace CCRift;

MAIN
{
	GLFWPreviewDevice *mDevice = &IDevice<GLFWPreviewDevice>::Instance();
	
#ifndef CCRIFT_MSW
    int* hinst = 0;
#endif
    
    mDevice->start(hinst);

	size_t demoDataSize = mDevice->preferredFrameHeight()
		* mDevice->preferredFrameWidth()
		* mDevice->preferredFrameDepth();

	unsigned char *demoData = new unsigned char[demoDataSize];

	memset(demoData, 0, demoDataSize);

	while (true)
	{
		if (!mDevice->isRunning())
		{
			mDevice->start(hinst);
		}

		for (int i = 0; i < demoDataSize; i++)
		{
			demoData[i] = rand() % 255;
		}

		mDevice->pushFrame(demoData);
		
		sleep(5000);
	}

	mDevice->stop();
	delete mDevice;

	return 0;
}
