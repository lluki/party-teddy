This project turns a boring lullaby playing toy into a veritable party machine.

It uses an ESP8266, drives the speaker via a transistor and 8 WS2812 LEDs

The software is written in Arduino and uses the (ESP8266 Audio)[https://github.com/earlephilhower/ESP8266Audio]
library.


== HTTP interface ==
At boot, the teddy will fetch /playlist which should return a single integer
describing how many songs can be fetched. Then it starts playing /songs/0.mp3,
if you push the button (that is mounted on the Teddys belly) it will loop to
/songs/1.mp3 etc. until it reaches the count described in playlist.
