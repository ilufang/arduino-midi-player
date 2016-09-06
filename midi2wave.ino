/*
 *	Midi2Wave
 *
 *	Implementation
 */


#include "midi2wave.h"

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

short ptr;

void setupMidi() {
	for (int i=0; i<WAVEBUF_SIZE; ++i)
		wave[i] = 0;
	ptr = 0;
}

void renderWaveBuffer() {
	//digitalWrite(12, HIGH);
	char note_count = 0;
	unsigned char leds = 0;
	for (int i=0; i<MAX_NOTE; ++i) {
		if (key_vels[i]) {
			if (!note_count) {
				key = i+1;
			}
			++note_count;
			leds |= 1<<(i%6);
		}
	}

	if (!note_count)
		for (int i=0; i<WAVEBUF_SIZE; ++i)
			wave[i] = 0;

	for (int i=0; i<MAX_NOTE; ++i) {
		if (key_vels[i]) {
			if (i==key-1)
				for (int j = 0; j < WAVEBUF_SIZE; ++j)
					wave[j] =  pgm_read_byte_near( sine+( j & ( SINE_SAMPLE_SIZE-1 ) ) )*key_vels[i]/16/note_count;
			else
				for (int j = 0; j < WAVEBUF_SIZE; ++j)
					wave[j] +=  pgm_read_byte_near( sine+( ( j * (int)pow( 1.0594630943592952645618252949463, i-key+1 ) ) & ( SINE_SAMPLE_SIZE-1 ) ) )*key_vels[i]/16/note_count;
		}
	}

	for (int i = 0; i < 7; ++i)
	{
		// Switch LEDs
		if (leds & (1<<i))
			sbi(PORTD, i+2);
		else
			cbi(PORTD, i+2);
	}

	//digitalWrite(12, LOW);
}

void loadNextEvent() {
	if (ptr >= SONG_LEN)
	{
		// Restart in 3s
		Serial.println("Ended.");
		setupMidi();
		event_length = 3000;
		return;
	}
	int new_length = NOTE_DELAY(ptr)*TEMPO;
	key_vels[NOTE_NUMBER(ptr)-1] = NOTE_VEL(ptr);

	++ptr;
	if (new_length == 0)
		loadNextEvent();
	else {
		renderWaveBuffer();
		event_length = new_length/4;

	}
}
