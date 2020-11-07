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

#ifndef _AUDIOOUTPUTI2SNODACBEATDETECT_H
#define _AUDIOOUTPUTI2SNODACBEATDETECT_H

#include "AudioOutputI2SNoDAC.h"
typedef void (*LED_ON)(bool);

class AudioOutputI2SNoDACBeatDetect : public AudioOutputI2SNoDAC
{
  public:
    LED_ON callback; 
    float power_th;
    AudioOutputI2SNoDACBeatDetect(LED_ON led_on_callback, float power_th, int port = 0);
    virtual ~AudioOutputI2SNoDACBeatDetect() override;
    virtual bool ConsumeSample(int16_t sample[2]) override;
};


#endif
