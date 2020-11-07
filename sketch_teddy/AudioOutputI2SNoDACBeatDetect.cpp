/*
  AudioOutputI2SNoDAC
  Audio player using SW delta-sigma to generate "analog" on I2S data
 
  Copyright (C) 2017  Earle F. Philhower, III

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include "dft.h"

#ifdef ESP32
  #include "driver/i2s.h"
#else
  #include <i2s.h>
#endif
#include "AudioOutputI2SNoDACBeatDetect.h"

static inline int32_t asm_ccount(void) {
    int32_t r;
    asm volatile ("rsr %0, ccount" : "=r"(r));
    return r;
}

AudioOutputI2SNoDACBeatDetect::AudioOutputI2SNoDACBeatDetect(LED_ON led_on_callback,float power_th, int port)
    : AudioOutputI2SNoDAC(port)
    
{
    this->callback = led_on_callback;
    this->power_th = power_th;
    dft_reset();
}

AudioOutputI2SNoDACBeatDetect::~AudioOutputI2SNoDACBeatDetect()
{
}

bool AudioOutputI2SNoDACBeatDetect::ConsumeSample(int16_t sample[2])
{ 
  bool res = AudioOutputI2SNoDAC::ConsumeSample(sample);

  MakeSampleStereo16( sample );
  dft_new_sample(sample[0]);
  if(dft_ready()){
    float res = dft_read_res();
    if(res > this->power_th) this->callback(true);
    else this->callback(false);
    dft_reset();
  }
  return res;
}
