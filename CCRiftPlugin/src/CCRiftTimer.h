#ifndef __CCRIFT_TIMER_H__
#define __CCRIFT_TIMER_H__

#include "CCRiftCommons.h"

#if defined( CCRIFT_MAC )
	#include <CoreFoundation/CoreFoundation.h>
#endif

namespace CCRift
{

	class Timer {
	public:
		Timer();
		Timer( bool startOnConstruction );
	
		void	start();
		double	getSeconds() const;
		void	stop();

		bool	isStopped() const { return mIsStopped; }
	
	  private:
		bool	mIsStopped;
	#if defined( CCRIFT_MAC )
		::CFAbsoluteTime	mStartTime, mEndTime;
	#elif (defined( CCRIFT_MSW) )
		double				mStartTime, mEndTime, mInvNativeFreq;
	#endif
	};

}

#endif /__CCRIFT_TIMER_H__