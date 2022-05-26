![badge](https://img.shields.io/badge/ESP8266-HARDWARE-blue?style=flat-square)

# wifi dmx + IR led lighting fixtures 
using ESP8266 for individually addressable LED bars controllable through both IR remote and DMX network


## what is it?

## how to use it? 

## use cases? 

## User Manual for the lights
Hello and welcome to the *magical* light fixtures that can be controlled via a handy 44 key infrared remote OR from a lighting console, computer, or mobile device via DMX lighting protocol. 

- DMX mode - set as 3 channel fixtures with 32 pixels in each fixture
    - color mode = RGB
- reset button: hold while turning on to reset the WIFI network to access point mode. 

## installation guide:
I am assuming you are using the classic Arduino IDE! 

make sure you install ESP8266 board add-ons for the IDE, [here](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/) is instructions on how to do that 

### required libraries: 
IOT and WIFI stuff: 
 - [IOTWebConf.h](https://github.com/prampec/IotWebConf) // NOTE: This library has been modified so that it stores in EEPROM starting at address 8 instead of 0, use the version included in this repository rather than the official release OR change the line "# define IOTWEBCONF_CONFIG_START 0" to "# define IOTWEBCONF_CONFIG_START 8" in IotWebConf.h in the official release
-also move this into your Arduino library folder

- [ArtnetWifi.h ](https://github.com/natcl/Artnet) - not to be confused with other artnet wifi libraries,  this is the one to use

LED stuff: 
- [Adafruit_NeoPixel.h](https://github.com/adafruit/Adafruit_NeoPixel)
- [FastLED](https://github.com/FastLED/FastLED)
  
Infrared (IR) reciever stuff: 
- [IRRemoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266)
-  [IRremote](https://github.com/NicoHood/IRLremote)  (outdated?)

Misc Libraries to include: 
- EEPROM.h , should be inheirited from the ESP8266 board library 

## pin notes for esp8266
fastLED pinout problem:
according to https://github.com/FastLED/FastLED/wiki/ESP8266-notes :
GPIO6, GPIO7, GPIO8, GPIO9, GPIO10, and GPIO11 are all unavailable to you.
On some boards, if you want to use GPIO15, you may need to follow the instructions here - http://www.instructables.com/id/ESP8266-Using-GPIO0-GPIO2-as-inputs/?ALLSTEPS

LEDPIN 16 // D0 built in
BUTTONPIN 13 // D7 on esp8266 dev boad
MODEPIN 12 // D6 on esp8266 dev board
ir recieiver = GPIO 14 or D5
led output =    2 //D4





