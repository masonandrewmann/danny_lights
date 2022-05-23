# danny_lights
esp8266 for individually addressable LED bars controllable through both IR remote and DMX network

#usage notes
 - DMX mode - set as 3 channel fixtures with 32 pixels in each fixture
		- color mode = RGB
 - reset button: hold while turning on to reset the WIFI network to access point mode. 

required libraries:

EEPROM.h

IOTWebConf.h // NOTE: This library has been modified so that it stores in EEPROM starting at address 8 instead of 0, use the version included in this repository rather than the official release OR change the line "# define IOTWEBCONF_CONFIG_START 0" to "# define IOTWEBCONF_CONFIG_START 8" in IotWebConf.h in the official release
-also move this into your Arduino library folder

ArtnetWifi.h 

Adafruit_NeoPixel.h


for IR remote code: use the IRremote library here: https://github.com/NicoHood/IRLremote

# pin notes for esp8266
fastLED pinout problem:
according to https://github.com/FastLED/FastLED/wiki/ESP8266-notes :
GPIO6, GPIO7, GPIO8, GPIO9, GPIO10, and GPIO11 are all unavailable to you.
On some boards, if you want to use GPIO15, you may need to follow the instructions here - http://www.instructables.com/id/ESP8266-Using-GPIO0-GPIO2-as-inputs/?ALLSTEPS

LEDPIN 16 // D0 built in
BUTTONPIN 13 // D7 on esp8266 dev boad
MODEPIN 12 // D6 on esp8266 dev board
ir recieiver = GPIO 14 or D5
led output =    2 //D4





