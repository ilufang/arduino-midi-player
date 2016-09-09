Arduino MIDI Player
===================

This program plays MIDI music on Arduino by generating analog/PWM waves on a port connected to a speaker/buzzer. We use timer 2 to do direct digital synthesis (DDS).

Features
--------

- **Chords/multiple notes!** The DDS can add multiple waves together on a single timer and single port. Unlike `tone()` which you can only run one frequency at any time.
- **Sine waves!** You can define any arbitrary function/wave sample to use as the instrument in `smf2seq.js`. No more square waves of the built-in `tone()` function.
- **Tested on UNO!** Though it might not work directly on other models, after some modification the concept should work on any Arduino. (since UNO is the crappiest model)


How to use
----------

**Hardware**

1. Connect buzzer/speaker to PWM port 11. Use a proper resister.
2. A variable-resistance resister is recommended to adjust the volume.
3. (Optional) Connect LEDs to port 2-7.
4. Ground everything.

**Software**

1. Place your `.mid` file under the main directory
2. Run `node smf2seq.js <your_midi_song.mid>` to generate `sequence.h` (You will need to install node.js if you don't have one)
3. Open `arduino-midi-player.ino` in Arduino IDE
4. Compile & Upload

Limitations
-----------

Due to the hardware of Arduino, your MIDI might not work perfectly. (Apparently an Arduino UNO will NEVER be able to handle a black MIDI)

- **Large files.** The max internal storage is 32KB and `smf2seq.js` use 6 bytes for each note.
- **Short time intervals/High BPM.** 1/2048 notes will not likely to play because of the internal clock
- **High/Low pitches.** The clock might not be fast enough to generate a desired wave of the specified frequency
- **Complex chords.** The clock interrupt cannot only process a limit number (4-6) of notes within the clock interval. Lower notes that exceed the limit of `KEYBUF_SIZE` will be discarded
- **Instruments.** MIDI instruments will be disregarded and everything will be sine waves

Notes
-----

- macros like `sbi`, `cbi`, `OCR2A` are used instead of API functions like `digitalWrite` to improve performance. Google them for more details
