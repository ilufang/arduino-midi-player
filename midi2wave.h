/*
 *	Midi2Wave
 *
 *	Playback controller
 *
 *	Load MIDI events into the global variable
 */

#ifndef __MIDI2WAVE_H__
#define __MIDI2WAVE_H__

#include "sequence.h"

#define MAX_NOTE 128
#define KEYBUF_SIZE 5
#define SINE_SAMPLE_SIZE 256

#define PIANO(key) (key?(pow(1.0594630943592952645618252949463,key+1-32-49+24)*440):0)

#define NOTE_NUMBER(index) pgm_read_byte_near(notes+index)
#define NOTE_DELAY(index) (pgm_read_word_near(params+index)>>4)
#define NOTE_VEL(index) (pgm_read_word_near(params+index)&15)

unsigned char volatile active_keys[KEYBUF_SIZE];
unsigned char volatile key_vels[MAX_NOTE];

volatile char note_count = 0;

// The current length in milliseconds
int event_length = 0;

// The primary key
char key = 61;

/*
 *	Setup
 *
 *	Initialize stuff
 */
void setupMidi();

/*
 *	Render wave buffer
 *
 *	Generate the wave buffer with current notes
 */
void renderWaveBuffer();

/*
 *	Load next event
 *
 *	Load the next midi note/chord
 *	Updates the next delay variable
 *	Updates LEDs
 */
void loadNextEvent();

#endif
