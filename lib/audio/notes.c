#include "notes.h"
#include <math.h>

const char* NOTE_NAMES[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

float note_to_frequency(uint8_t midi_note)
{
	// A4 (MIDI note 69) = 440 Hz
	return 440.0f * powf(2.0f, (midi_note - 69) / 12.0f);
}

float note_name_to_frequency(int note_index, int octave)
{
	// Convert to MIDI note: C4 = 60
	int midi_note = 60 + (octave - 4) * 12 + note_index;
	return note_to_frequency(midi_note);
}

const char* get_note_name(int note_index)
{
	if (note_index < 0 || note_index >= 12) {
		return "?";
	}
	return NOTE_NAMES[note_index];
}
