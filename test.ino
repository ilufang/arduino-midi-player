/*
 *	Main
 *
 *	Setup Arduino and use timer2 to output sine wave
 */

/*
 * Part of this file contains code modified from
 * http://interface.khm.de/index.php/lab/interfaces-advanced/arduino-dds-sinewave-generator/
 *
 * DDS Sine Generator mit ATMEGS 168
 * Timer2 generates the  31250 KHz Clock Interrupt
 *
 * KHM 2009 /  Martin Nawrath
 * Kunsthochschule fuer Medien Koeln
 * Academy of Media Arts Cologne
 */

#include "avr/pgmspace.h"

#include "midi2wave.h"

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define POW2_32 4294967296
#define refclk 31376.6 // Reference clock

// variables in interrupt service
volatile int timer_tick = 0; // seeking position in wave
volatile unsigned char timer_micro = 0; // timing counter in microseconds
volatile unsigned short timer_milli = 0; // timing counter in milliseconds
volatile unsigned long phaccu; // phase accumulator
volatile unsigned long tword_m; // DDS tuning word m

void setup()
{
	Serial.begin(9600);
	Serial.println("Hello");
	for (int i = 2; i <= 8; ++i)
		pinMode(i,OUTPUT); // LED output

	pinMode(11,OUTPUT); // PWM Wave output
	setupMidi();
	setupTimer2();
}

void loop()
{
	while(true) {
		if (timer_milli > event_length) { // wait for the next midi event
			cbi (TIMSK2,TOIE2);
			loadNextEvent();
			timer_milli=0;
			tword_m=POW2_32*PIANO(key)/refclk;  // calculate new DDS tuning word
			sbi (TIMSK2,TOIE2);
		}
	}
}

// timer2 setup
// set pre-scaler to 1, PWM mode to phase correct PWM,  16000000/510 = 31372.55 Hz clock
void setupTimer2() {

	// Timer2 Clock Pre-scaler to : 1
	sbi (TCCR2B, CS20);
	cbi (TCCR2B, CS21);
	cbi (TCCR2B, CS22);

	// Timer2 PWM Mode set to Phase Correct PWM
	cbi (TCCR2A, COM2A0); // clear Compare Match
	sbi (TCCR2A, COM2A1);

	sbi (TCCR2A, WGM20); // Mode 1 / Phase Correct PWM
	cbi (TCCR2A, WGM21);
	cbi (TCCR2B, WGM22);

	// initialize DDS tuning word
	tword_m=POW2_32*PIANO(key)/refclk;

	// disable Timer0 interrupts to avoid timing distortion
	cbi (TIMSK0,TOIE0);
	// start Timer2!
	sbi (TIMSK2,TOIE2);

}

/*
 * Timer2 Interrupt Service
 *
 * Running at 31372,550 KHz = 32uSec
 * this is the timebase REFCLOCK for the DDS generator
 * FOUT = (M (REFCLK)) / (2 exp 32)
 * runtime : 8 microseconds ( inclusive push and pop)
 */
ISR(TIMER2_OVF_vect) {
	phaccu+=tword_m*4; // soft DDS, phase accumulator with 32 bits
	timer_tick=phaccu >> 24; // use upper 10 bits for phase accumulator as frequency information

	// Write to PWM port 11
	OCR2A=wave[timer_tick];
	// OCR2A = pgm_read_byte_near(sine+timer_tick);

	++timer_micro;
	if(timer_micro > 125) {
		++timer_milli;
		timer_micro=0;
	}
}
