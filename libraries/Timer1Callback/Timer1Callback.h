// Timer1Callback.h

#ifndef _TIMER1CALLBACK_h
#define _TIMER1CALLBACK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Timer1Callback {
public:
	virtual void isrOvf();
	virtual void isrCompa();
	virtual void isrCapt();
};


#endif

