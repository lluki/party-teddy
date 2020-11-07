
#ifndef DFT_H
#define DFT_H

#include <cstdint>

#define FREQ 50.f // Center Frequency
#define RATE 44100.f // Sample Rate
#define WINDOW 200 // Number Of Samples Per Window
#define UNDERSAMPLE 10

void dft_reset();
void dft_new_sample(int16_t sample);
float dft_read_res();
bool dft_ready();

#endif
