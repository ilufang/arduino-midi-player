/*
 *	Midi2Wave
 *
 *	Playback controller
 *
 *
 */

#ifndef __MIDI2WAVE_H__
#define __MIDI2WAVE_H__

#include "sequence.h"

#define MAX_NOTE 128
#define WAVEBUF_SIZE 1024
#define SINE_SAMPLE_SIZE 256

#define PIANO(key) (pow(1.0594630943592952645618252949463,key-32-49)*440)

#define NOTE_NUMBER(index) pgm_read_byte_near(notes+index)
#define NOTE_DELAY(index) (pgm_read_word_near(params+index)>>4)
#define NOTE_VEL(index) (pgm_read_word_near(params+index)&15)

unsigned char volatile wave[WAVEBUF_SIZE];

unsigned char key_vels[MAX_NOTE];

// The current length in milliseconds
int event_length = 1000;

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
 *	Load the next midi note/chord into the wave buffer
 *	Updates the next delay variable
 *	Updates LEDs
 */
void loadNextEvent();

#endif
