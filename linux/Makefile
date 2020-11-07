OPTS=`pkg-config --cflags --libs libpulse-simple` -I..
fft-play: main.cpp ../dft.cpp
	g++ main.cpp ../dft.cpp $(OPTS) -o fft-play

run: fft-play
	sox ~/Downloads/wap_full.mp3 -t raw -r 44100 -b 16 -e signed-integer -c 1 - | ./fft-play
	#sox ~/Downloads/bensound-ukulele.mp3 -t raw -r 44100 -b 16 -e signed-integer -c 1 - | ./fft-play

clean:
	rm -f fft-play
