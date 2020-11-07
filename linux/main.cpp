#include <iostream>
#include <cstdint>
#include <pulse/simple.h>
#include "dft.h"


int main(){
    pa_simple *s;
    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16NE;
    ss.channels = 1;
    ss.rate = 44100;
    s = pa_simple_new(NULL,               // Use the default server.
                  "fft-play",           // Our application's name.
                  PA_STREAM_PLAYBACK,
                  NULL,               // Use the default device.
                  "Music",            // Description of our stream.
                  &ss,                // Our sample format.
                  NULL,               // Use default channel map
                  NULL,               // Use default buffering attributes.
                  NULL               // Ignore error code.
                  );

    dft_reset();
    while(1){
       int16_t sample;
       if(fread(&sample, sizeof(sample), 1, stdin) != 1)
           break;
       dft_new_sample(sample);
       if(dft_ready()){
        float res = dft_read_res();
        if(res > 10.0) printf("DFT %f\n", res);
        dft_reset();
       }
       pa_simple_write(s, &sample, sizeof(sample), NULL);
    }
}
