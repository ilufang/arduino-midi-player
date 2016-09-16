/*
 *	Arduino MIDI Player
 *
 *	Setup Arduino and use timer2 to synthesize and output sine wave
 *
 *	2016 by ilufang
 */

/*
 * Part of this file contains code modified/referenced from
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
volatile unsigned long phaccu_1, phaccu_2, phaccu_3, phaccu_4, phaccu_5, phaccu_6, phaccu_7, phaccu_8; // phase accumulator
volatile unsigned long tword_m_1, tword_m_2, tword_m_3, tword_m_4, tword_m_5, tword_m_6, tword_m_7, tword_m_8; // DDS tuning word m
unsigned long phaccu_all;

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
			// calculate new DDS tuning word
			tword_m_1=POW2_32*PIANO(active_keys[0])/refclk;
			tword_m_2=POW2_32*PIANO(active_keys[1])/refclk;
			tword_m_3=POW2_32*PIANO(active_keys[2])/refclk;
			tword_m_4=POW2_32*PIANO(active_keys[3])/refclk;
			if (!tword_m_1) phaccu_1 = 0;
			if (!tword_m_2) phaccu_2 = 0;
			if (!tword_m_3) phaccu_3 = 0;
			if (!tword_m_4) phaccu_4 = 0;
			timer_milli=0;
			sbi (TIMSK2,TOIE2);
		}
	}
}

/*
 * timer2 setup
 *
 * set pre-scaler to 1, PWM mode to phase correct PWM,  16000000/510 = 31372.55 Hz clock
 */
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
	tword_m_1=0;
	tword_m_2=0;
	tword_m_3=0;
	tword_m_4=0;

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
	// soft DDS, phase accumulator with 32 bits
	phaccu_1 += tword_m_1;
	phaccu_2 += tword_m_2;
	phaccu_3 += tword_m_3;
	phaccu_4 += tword_m_4;
	phaccu_5 += tword_m_5;

	// use upper 8 bits for phase accumulator as frequency information
	int phaccu_all = sine[phaccu_1>>24];
	phaccu_all += sine[phaccu_2>>24];
	phaccu_all += sine[phaccu_3>>24];
	phaccu_all += sine[phaccu_4>>24];
	phaccu_all += sine[phaccu_5>>24];

	// Write to PWM port 11
	OCR2A = phaccu_all/KEYBUF_SIZE;

	// Increment timing counter
	if(++timer_micro == 31) {
		++timer_milli;
		timer_micro=0;
	}
}
