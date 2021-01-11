# danny_lights
esp8266 for individually addressable LED bars controllable through both IR remote and DMX network

required libraries:

EEPROM.h

IOTWebConf.h // NOTE: This library has been modified so that it stores in EEPROM starting at address 8 instead of 0, use the version included in this repository rather than the official release OR change the line "# define IOTWEBCONF_CONFIG_START 0" to "# define IOTWEBCONF_CONFIG_START 8" in IotWebConf.h in the official release

ArtnetWifi.h 

Adafruit_NeoPixel.h


for IR remote code: use the IRremote library here: https://github.com/NicoHood/IRLremote
