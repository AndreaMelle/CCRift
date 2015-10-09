#include "CCRiftTimer.h"

using namespace CCRift;

#if (defined( CCRIFT_MSW ) )
	#include <windows.h>
#endif

Timer::Timer()
	: mIsStopped( true )
{
#if defined( CCRIFT_MAC )
	mEndTime = mStartTime = -1;
#elif (defined( CCRIFT_MSW ) )
	::LARGE_INTEGER nativeFreq;
	::QueryPerformanceFrequency( &nativeFreq );
	mInvNativeFreq = 1.0 / nativeFreq.QuadPart;
	mStartTime = mEndTime = -1;
#endif
}

Timer::Timer( bool startOnConstruction )
	: mIsStopped( true )
{
#if defined( CCRIFT_MAC )
		mEndTime = mStartTime = -1;
#elif (defined( CCRIFT_MSW ) )
	::LARGE_INTEGER nativeFreq;
	::QueryPerformanceFrequency( &nativeFreq );
	mInvNativeFreq = 1.0 / nativeFreq.QuadPart;
	mStartTime = mEndTime = -1;
#endif
	if( startOnConstruction ) {
		start();
	}
}

void Timer::start()
{	
#if defined( CCRIFT_MAC )
	mStartTime = ::CFAbsoluteTimeGetCurrent();
#elif (defined( CCRIFT_MSW ) )
	::LARGE_INTEGER rawTime;
	::QueryPerformanceCounter( &rawTime );
	mStartTime = rawTime.QuadPart * mInvNativeFreq;
#endif

	mIsStopped = false;
}

double Timer::getSeconds() const
{
	if( mIsStopped )
		return mEndTime - mStartTime;
	else {
#if defined( CCRIFT_MAC )
		return ::CFAbsoluteTimeGetCurrent() - mStartTime;
#elif (defined( CCRIFT_MSW ) )
	::LARGE_INTEGER rawTime;
	::QueryPerformanceCounter( &rawTime );
	return (rawTime.QuadPart * mInvNativeFreq) - mStartTime;
#endif
	}
}

void Timer::stop()
{
	if( ! mIsStopped ) {
#if defined( CCRIFT_MAC )
		mEndTime = ::CFAbsoluteTimeGetCurrent();
#elif (defined( CCRIFT_MSW ) )
		::LARGE_INTEGER rawTime;
		::QueryPerformanceCounter( &rawTime );
		mEndTime = rawTime.QuadPart * mInvNativeFreq;
#endif
		mIsStopped = true;
	}
}

