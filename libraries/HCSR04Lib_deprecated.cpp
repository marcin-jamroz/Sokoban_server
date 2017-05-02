/*
 Name:		HCSR04Lib.cpp
 Created:	01.05.2017 22:59:52
 Author:	Jaroslaw Butajlo
 Editor:	http://www.visualmicro.com
*/
/*
#include "HCSR04Lib.h"

HCSR04::HCSR04(int trigPin, int echoPin) {
	pinMode(trigPin, OUTPUT);
	this->trigPin = trigPin;
	this->echoPin = echoPin;
	cli();
	TCCR1A = 0; // Timer/Counter Control Register 1 A - reset
	TCCR1B = 0;
//	TCCR1B |= (_BV(WGM13)); // phase and freq corrected mode
	// After reseting Control Register Timer works in Normal Mode - can generate Overflow interrputs 
	TIMSK1 |= (_BV(TOIE1));
}

void HCSR04::risingEdge() {
	cli(); // disabling interrupts
	TCNT1 = 0; // reset counter register
	TCCR1B |= (_BV(CS12) | _BV(CS10)); // timer prescaling / 1024 and starts the timer
	sei(); // enabling interrupts

}
void HCSR04::fallingEdge() {
	cli();
	// Reset and stop the Timer1
	TCCR1B &= ~(_BV(CS12) | _BV(CS10)); // timer stop 
	pulseWidth = TCNT1 + (TIMER1MAX + 1) * (unsigned long)timerOvfCounter;
	timerOvfCounter = 0;

	Serial.print("Distance (cm): ");
	Serial.println(getLatestMeasure());
	isMeasureOn = false;


}

void HCSR04::counterOverflow() {
	timerOvfCounter++;
}

void HCSR04::startMeasuring() {
	isMeasureOn = true;
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10); // 10us TTL signal (width from documentation)
	digitalWrite(trigPin, LOW);
}

unsigned long HCSR04::getLatestMeasure() {
	return (pulseWidth * 1000000L / (F_CPU / 1024L) /2L) / 58L;
}
bool HCSR04::isMeasuring() {
	return isMeasureOn;
}
*/