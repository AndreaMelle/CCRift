#include "CCRiftIDevice.h"

using namespace CCRift;
using namespace std;

Process::Process()
    : mThreadCallback(nullptr)
    , mRunning(false)
{

}

Process::~Process()
{
	this->stop();
}

void Process::start()
{
	this->stop();

	if (mThreadCallback != nullptr)
	{
		mRunning = true;
		mThread = shared_ptr<thread>(new thread(mThreadCallback));
	}
}

void Process::stop()
{
	mRunning = false;
	if (mThread)
	{
		mThread->join();
		mThread.reset();
	}
}