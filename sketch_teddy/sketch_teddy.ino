#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

#include <ESP8266HTTPClient.h>

#include "AudioFileSourceICYStream.h"
#include "AudioFileSourceBuffer.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDACBeatDetect.h"

// To run, set your ESP8266 build to 160MHz, update the SSID info, and upload.

// WIFI SETUP
#ifndef STASSID
#define STASSID "YOUR-WIFI_SSID"
#define STAPSK  "YOUR-WIFI-PASSWORD"
#endif

// PLAYLIST SETUP
#define PLAYLIST_INIT -1
#define NEXT_PLAYLIST_PIN 0
#define SWITCH_ON LOW
#define SWITCH_OFF HIGH


// LED SETUP
#define LED_PIN 14
#define PIXELS_NO 8
#define LED_ON HIGH
#define LED_OFF LOW
#define RGB_MAX_VAL 256

//BEAT DETECT SETUP
#define BEAT_POWER_TH 100.0

// PASSWORD CNSTS
const char* ssid = STASSID;
const char* password = STAPSK;


//PLAYLIST CNSTS
const char URL_PLAYLIST[] ="http://YOUR-SERVER-IP:8000/playlist";
const char URL_SONGS[] = "http://YOUR-SERVER-IP:8000/songs/%d.mp3";


//AUDIO VARIABLES
AudioFileSourceICYStream *file = NULL;
AudioGeneratorMP3 *mp3  = NULL;
AudioFileSourceBuffer *buff  = NULL;
AudioOutputI2SNoDACBeatDetect *out  = NULL;


// PLAYLIST VARIABLES
int playlist_iterator = 0;
int playlist_count = PLAYLIST_INIT;


//PIXEL VARIABLES
Adafruit_NeoPixel pixels(PIXELS_NO, LED_PIN, NEO_GRB + NEO_KHZ800);
const auto OFF_COLOR = pixels.Color(0,0,0);
static uint16_t CLR = 0;


// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  (void) isUnicode; // Punt this ball for now
  // Note that the type and string may be in PROGMEM, so copy them to RAM for printf
  char s1[32], s2[64];
  strncpy_P(s1, type, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  strncpy_P(s2, string, sizeof(s2));
  s2[sizeof(s2)-1]=0;
  Serial.printf("METADATA(%s) '%s' = '%s'\n", ptr, s1, s2);
  Serial.flush();
}


// Called when there's a warning or error (like a buffer underflow or decode hiccup)
void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  // Note that the string may be in PROGMEM, so copy it to RAM for printf
  char s1[64];
  strncpy_P(s1, string, sizeof(s1));
  s1[sizeof(s1)-1]=0;
  Serial.printf("STATUS(%s) '%d' = '%s'\n", ptr, code, s1);
  Serial.flush();
}



//called to light up the LED once the beat is detected
void beatCallback(bool isOn) {
  
  pixels.clear();
  auto color = OFF_COLOR;
  if (isOn) {
     color = pixels.ColorHSV(CLR);
     CLR = CLR + 128;
  }
  for (int i = 0; i < PIXELS_NO; i++) {
     pixels.setPixelColor(i, color);
  }
  pixels.show();
}


int playlist()
{
  HTTPClient http;
  http.begin(URL_PLAYLIST);
  int httpCode = http.GET();
  int playlist_count = PLAYLIST_INIT;
  if (httpCode > 0) 
  { //Check the returning code
      String payload = http.getString();
      playlist_count = payload.toInt();
  }
 
  http.end(); //Close connection 
  return playlist_count; 
}


char * song(int iterator)
{
    static char song_it[sizeof(URL_SONGS)+2] = {'\0'};
    int n = snprintf(song_it, sizeof(song_it), URL_SONGS, iterator);
    return song_it;
   
}


void play(int iterator) {
  int effective_iterator = iterator % playlist_count;
  Serial.printf("playing song %d\n", effective_iterator);
  const char * URL = song(effective_iterator);
  Serial.printf("Fetching url %s\n", URL);
  audioLogger = &Serial;
  if(file) {
    delete file;
    delay(1000);
  }

  file = new AudioFileSourceICYStream(URL);
  file->RegisterMetadataCB(MDCallback, (void*)"ICY");
  if(buff) delete buff;
  buff = new AudioFileSourceBuffer(file, 2*2048);
  buff->RegisterStatusCB(StatusCallback, (void*)"buffer");
  if(out) delete out;
  out = new AudioOutputI2SNoDACBeatDetect(&beatCallback, BEAT_POWER_TH);
  if(mp3) delete mp3;
  mp3 = new AudioGeneratorMP3();
  mp3->RegisterStatusCB(StatusCallback, (void*)"mp3");
  mp3->begin(buff, out);
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Connecting to WiFi");

  //pinMode(LED_PIN, OUTPUT); 
  //digitalWrite(LED_PIN, LED_OFF); 

  pinMode(NEXT_PLAYLIST_PIN, INPUT);

  
  WiFi.disconnect();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);

  // Try forever
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...Connecting to WiFi");
    delay(1000);
    pixels.begin();
  }
  Serial.println("Connected");
  
  // Try forever to get the playlist count
  while (playlist_count <= 0) {
    Serial.println("Fetching playlist count...");
    playlist_count = playlist();
  }

  Serial.printf("Playlist count of %d songs\n", playlist_count);
  
  play(playlist_iterator);
  
}

int next;
void loop()
{
  static int lastms = 0;
  next = digitalRead(NEXT_PLAYLIST_PIN);

  if (next == SWITCH_ON) {
    Serial.printf("Stopping mp3");
    mp3->stop();
    playlist_iterator ++;
    play(playlist_iterator);
    delay(1000);
  }
  else {
        
        if (mp3->isRunning()) {
          
          if (!mp3->loop()) {
            Serial.printf("Loop done?!\n");
            mp3->stop();
          }
        } else {
          mp3->stop();
          Serial.printf("MP3 done\n");
          playlist_iterator ++;
          play(playlist_iterator);
          delay(1000);
        }
  }
}
