#ifndef NOTES_H
#define NOTES_H

#include <stdint.h>

// Convert MIDI note number to frequency in Hz
float note_to_frequency(uint8_t midi_note);

// Convert note name and octave to frequency
float note_name_to_frequency(int note_index, int octave);

// Note names for display
extern const char* NOTE_NAMES[12];

// Get note name by index (same as NOTE_NAMES[index] with bounds checking)
const char* get_note_name(int note_index);

#endif  // NOTES_H