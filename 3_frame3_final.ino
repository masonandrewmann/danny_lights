// 2.21.2020 taken from the artnet neopixel code and added the wifi and dotstar stuff. needs a little altering but it is close.
//5.14.2020 added a wifiWorkingLights indicator
//7.8.2020 porting over for RGBW leds
//TODO
//add wifi sleepydog reconnecting thing from TMZ stuff

/*
  This example will receive multiple universes via Artnet and control a strip of ws2811 leds via
  Adafruit's NeoPixel library: https://github.com/adafruit/Adafruit_NeoPixel
  This example may be copied under the terms of the MIT license, see the LICENSE file for details
*/

#include <Artnet.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>

#include <WiFi101.h>
#include <WiFiUdp.h>
#include <Adafruit_SleepyDog.h>


// Neopixel settings
const int numLeds = 284; // Tile 3 !
const int channelsPerLed = 4; // tile1 = RGBW
const int numberOfChannels = numLeds * channelsPerLed; // Total number of channels you want to receive (1 led = 3 channels)
const byte dataPin = 13;
Adafruit_NeoPixel leds = Adafruit_NeoPixel(numLeds, dataPin, NEO_GRBW + NEO_KHZ800);
int status = WL_IDLE_STATUS;

char ssid[] = "foaw_frames"; //  your network SSID (name)
char pass[] = "framesoup";    // your network password (use for WPA, or use as key for WEP)
//char ssid[] = "B2"; //  your network SSID (name)
//char pass[] = "77037880108";    // your network password (use for WPA, or use as key for WEP)
long dbm;


// Artnet settings
Artnet artnet;
const int startUniverse = 5; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.


// Check if we got all universes
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

// Change ip and mac address for your setup
byte ip[] = {192, 168, 1, 151};
//byte mac[] = {0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC};
//byte broadcast[] = {10, 0, 1, 255};
void setup()
{
  powerTest();
  wifiInit();
  Serial.begin(9600);
  leds.begin();
  // artnet.setBroadcast(broadcast);
  artnet.begin();
  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
  int countdownMS = Watchdog.enable(10000);

}

void loop()
{
  artnet.read();
  wifiCheck();
}

void wifiCheck() {
  if (WiFi.status() == WL_CONNECTED) {
    Watchdog.reset();
  }
  dbm = WiFi.RSSI();
  Serial.print("signal Strength: ");
  Serial.println(dbm);
  if (dbm <= -85) {
    //goToSleep();  //010419
    wifiInit2();
  }
}
void wifiInit() {
  WiFi.setPins(8, 7, 4, 2);
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(4000);
  }
  if (status == WL_CONNECTED) {
    wifiWorkingLights();
  }
  printWifiStatus();
}

void wifiInit2() {
  WiFi.setPins(8, 7, 4, 2);
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(4000);
  }
  if (status == WL_CONNECTED) {
//    ledsBlink(255, 0, 255, 10, 5); //Blink purple 3 times once successfully connected (feel free to change the first three ints to whatever color value)
  }
  printWifiStatus();
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data, IPAddress remoteIP)
{
  // Serial.println(universe);
  sendFrame = 1;
  // set brightness of the whole strip
  if (universe == 15)
  {
    leds.setBrightness(data[0]);
    leds.show();
  }

  // Store which universe has got in
  if ((universe - startUniverse) < maxUniverses)
    universesReceived[universe - startUniverse] = 1;

  for (int i = 0 ; i < maxUniverses ; i++)
  {
    if (universesReceived[i] == 0)
    {
      //Serial.println("Broke");
      sendFrame = 0;
      break;
    }
  }

  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / channelsPerLed; i++)
  {
    //    int led = i + (universe - startUniverse) * (previousDataLength / channelsPerLed);
    //    if (led < numLeds) {
    //      if (channelsPerLed == 4)
    //      leds.setPixelColor(i, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    //      if (channelsPerLed == 3)
    if (universe == 5)  //*******ADDED 022320***** ALLOWED FOR SEPARATION OF UNIVERSES//
      leds.setPixelColor(i, data[i * channelsPerLed ], data[i * channelsPerLed + 1], data[i * channelsPerLed + 2], data[i * channelsPerLed + 3]);
    if (universe == 6)  //*******ADDED 022320***** ALLOWED FOR SEPARATION OF UNIVERSES//
      leds.setPixelColor(i + 128, data[i * channelsPerLed], data[i * channelsPerLed + 1], data[i * channelsPerLed + 2], data[i * channelsPerLed + 3]);
    if (universe == 7)  //*******ADDED 022320***** ALLOWED FOR SEPARATION OF UNIVERSES//
      leds.setPixelColor(i + 256, data[i * channelsPerLed], data[i * channelsPerLed + 1], data[i * channelsPerLed + 2], data[i * channelsPerLed + 3]);
  }
  //}
  previousDataLength = length;

  if (sendFrame)
  {
    leds.show();
    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }
}

void powerTest()
{
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 0, 0, 30);
  leds.show();
  delay(300);
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 0, 0, 0);
  leds.show();
  delay(300);
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 0, 0, 30);
  leds.show();
  delay(300);
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 0, 0, 0);
  leds.show();
}

void wifiWorkingLights()
{
  for (int i = 0 ; i < (numLeds / 4)  ; i++)
    leds.setPixelColor(i, 0, 0, 0, 30);
  leds.show();
  delay(500);
  for (int i = 0 ; i < (numLeds / 4)  ; i++)
    leds.setPixelColor(i, 0, 0, 0, 0);
  leds.show();
  delay(500);
  for (int i = numLeds / 2 ; i < (numLeds / 2) + numLeds / 4  ; i++)
    leds.setPixelColor(i, 0, 0, 0, 30);
  leds.show();
  delay(500);
  for (int i = numLeds / 2 ; i < (numLeds / 2) + numLeds / 4  ; i++)
    leds.setPixelColor(i, 0, 0, 0, 0);
  leds.show();
}

//-------------PRINT WIFI STATUS--------------//
void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
//-------------END OF PRINT WIFI STATUS--------------//


void ledsBlink(uint8_t r, uint8_t g, uint8_t b, long interval, uint8_t reps) {
  for (int t = 0; t < reps; t++) {    //Blink number of times
    delay(interval);
    for (int j = 0; j < 8; j++) {
      for (int i = 0 ; i < numLeds ; i++) {
        leds.setPixelColor(j * numLeds + i, leds.Color(r, g, b));
      }
    }
    leds.show();
    delay(interval);
    //Go black
    for (int j = 0; j < 8; j++) {
      for (int i = 0 ; i < numLeds ; i++) {
        leds.setPixelColor(j * numLeds + i, leds.Color(0, 0, 0));
      }
    }
    leds.show();
  }
}
