#include "scales.h"

// Musical scales
static const char* scale_names[] = { "Major", "Minor", "Dorian", "Phrygian", "Mixolydian", "Pentatonic" };

// Scale intervals (semitones from root)
static const int scales[][8] = {
	{ 0, 2, 4, 5, 7, 9, 11, 12 },  // Major
	{ 0, 2, 3, 5, 7, 8, 10, 12 },  // Minor
	{ 0, 2, 3, 5, 7, 9, 10, 12 },  // Dorian
	{ 0, 1, 3, 5, 7, 8, 10, 12 },  // Phrygian
	{ 0, 2, 4, 5, 7, 9, 10, 12 },  // Mixolydian
	{ 0, 2, 4, 7, 9, 12, -1, -1 }  // Pentatonic (only 6 notes)
};

static const char* note_names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

int get_scale_count(void)
{
	return sizeof(scale_names) / sizeof(scale_names[0]);
}

const char* get_scale_name(int scale_index)
{
	if (scale_index < 0 || scale_index >= get_scale_count()) {
		return "Unknown";
	}
	return scale_names[scale_index];
}

const int* get_scale_intervals(int scale_index)
{
	if (scale_index < 0 || scale_index >= get_scale_count()) {
		return scales[0];  // Default to Major
	}
	return scales[scale_index];
}

const char* get_note_name(int note_index)
{
	if (note_index < 0 || note_index >= 12) {
		return "?";
	}
	return note_names[note_index];
}
