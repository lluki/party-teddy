#include "dft.h"
#include <cmath>
//Inspired by: Goertzel DFT By Damian Peckett 2015

static const float coeff = 2.f*cos((2.f * 3.14159f * FREQ) / (RATE/UNDERSAMPLE));
static float Q0 = 0, Q1 = 0, Q2 = 0;
static int sample_ct = 0;

void dft_reset(){
    Q0 = 0;
    Q1 = 0;
    Q2 = 0;
    sample_ct = 0;
}

void dft_new_sample(int16_t sample) {
    if(sample_ct++ % UNDERSAMPLE == 0){
        Q0 = coeff * Q1 - Q2 + (float)(sample)/(2.f*INT16_MAX);
        Q2 = Q1; Q1 = Q0;
    }
}

bool dft_ready() {
    return sample_ct == WINDOW * UNDERSAMPLE;
}

float dft_read_res() {
    return Q2*Q2 + Q1*Q1 - coeff*Q1*Q2;
}

