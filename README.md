This project turns a boring lullaby playing toy into a veritable party machine.
Videos and pictures can be found on [hackaday.io](https://hackaday.io/project/175761-party-teddy/details/185859)

It uses an ESP8266, drives the speaker via a transistor and 8 WS2812 LEDs

The software is written in Arduino and uses the [ESP8266 Audio](https://github.com/earlephilhower/ESP8266Audio)
library.

### Hardware
A custom board using an ESP8266 12E, the schematics can be found [here](https://github.com/lluki/party-teddy/blob/main/schema/schema.pdf)

### HTTP interface
At boot, the teddy will fetch /playlist which should return a single integer
describing how many songs can be fetched. Then it starts playing /songs/0.mp3,
if you push the button (that is mounted on the Teddys belly) it will loop to
/songs/1.mp3 etc. until it reaches the count described in playlist.
