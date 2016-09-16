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
	for (int i=0; i<MAX_NOTE; ++i)
		key_vels[i] = 0;
	for (char i=0; i<KEYBUF_SIZE; ++i)
		active_keys[i] = 0;
	ptr = 0;
}

void renderWaveBuffer() {
	unsigned char leds = 0;
	note_count = 0;
	// Update active key buffer with new keys
	// Preserve original index if the key is not released
	for (char i = 0; i < KEYBUF_SIZE; ++i)
		if (key_vels[active_keys[i]]) {
			key_vels[active_keys[i]] |= 32; // the 6th bit is used as a "key-not-released" flag
			leds |= 1<<(active_keys[i]%6);
			++note_count;
		}
		else {
			active_keys[i] = 0; // Clear keys that are released
		}

	for (int i=MAX_NOTE-1; i>=0; --i) // Goes down: make sure the most significant note gets played
		if ( key_vels[i] && !(key_vels[i]&32) ) {
			for (char j=0; j<KEYBUF_SIZE; j++)
				if (!active_keys[j])
				{
					active_keys[j] = i;
					++note_count;
					break;
				}
			leds |= 1<<(i%6);
		}

	for (char i = 0; i < KEYBUF_SIZE; ++i)
		if (active_keys[i])
			key_vels[active_keys[i]] &= 31; // Clear out the 6th bit "key-not-released" flag

	// Set LEDs
	for (char i = 0; i < 7; ++i)
		if (leds & (1<<i))
			sbi(PORTD, i+2);
		else
			cbi(PORTD, i+2);
}

void loadNextEvent() {
	if (ptr >= SONG_LEN)
	{
		// Restart in 3s
		Serial.println("Ended.");
		setupMidi();
		event_length = 3000;
		for (int i = 0; i < 7; ++i)
			sbi(PORTD, i+2);
		return;
	}

	int new_length = NOTE_DELAY(ptr)*TEMPO;
	key_vels[NOTE_NUMBER(ptr)-1] = NOTE_VEL(ptr);

	++ptr;
	if (new_length == 0)
		loadNextEvent();
	else {
		renderWaveBuffer();
		event_length = new_length;
	}
}
