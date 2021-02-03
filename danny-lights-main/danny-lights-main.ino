//from wifi config code
#include <EEPROM.h>
#include <IotWebConf.h>
//from tmz artnet stuff
#include <ArtnetWifi.h> // https://github.com/natcl/Artnet  
#include <Adafruit_NeoPixel.h>


#define LEDPIN 16 //built in
#define BUTTONPIN 13 // D7 on esp8266 dev boad
#define IOTWEBCONF_CONFIG_START 8 // starting address for eeprom

//EEPROM addresses
int addr_universeNum = 0;
int addr_dmxAdr = 4;

int universeNum;
int dmxAdr;

// physical control
boolean programMode = false;

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "AutoconnectAP";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "password";

DNSServer dnsServer;
WebServer server(80);

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);

// Neopixel settings
const int numLeds = 10; // Tile 3 !
const int channelsPerLed = 3; // tile1 = RGBW
const int numberOfChannels = numLeds * channelsPerLed; // Total number of channels you want to receive (1 led = 3 channels)
const int dataPin = 14; //
Adafruit_NeoPixel leds = Adafruit_NeoPixel(numLeds, dataPin, NEO_GRB + NEO_KHZ800);
int status = WL_IDLE_STATUS;

char ssid[] = "NETGEAR08"; //  your network SSID (name)
char pass[] = "deepink618";    // your network password (use for WPA, or use as key for WEP)
long dbm;

// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.


// Check if we got all universes
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

// Change ip and mac address for your setup
byte ip[] = {192, 168, 0, 13};
//byte mac[] = {0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC};
//byte broadcast[] = {10, 0, 1, 255};


String getPage(){
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 01 Minimal</title></head><body>hiiii i am a light fixture,, welcome to my brain!<br>";
  s += "Go <a href='config'>here</a> to change network settings.";
  s += "<title>Light Bar</title>";
  s += "<style> body { background-color: #fffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  s += "</head><body><h1>DMX Configuration</h1>";
  s += "<h3>Universe</h3>";
  s += "<form action='/submit' method='POST'>";
  s += "<ul><li><input type='number' name='Uni' id='Uni' value='" + String(universeNum) + "' min='0' max='24'>";
  s += "</li></ul>";
  s += "<br>";
  s += "<h3>Address</h3>";
  s += "<form action='submit' method='POST'>";
  s += "<ul><li><input type='number' name='Adr' id = 'Adr' value ='" + String(dmxAdr) + "' min='0' max='24'>";
  s += "<br><br><br>";
  s += "<input type = 'submit' value='Submit'></li></ul>";
  s += "</body></html>\n";
  return s;
}

void writeIntIntoEEPROM(int address, int number)
{ 
  EEPROM.write(address, (number >> 24) & 0xFF);
  EEPROM.write(address + 1, (number >> 16) & 0xFF);
  EEPROM.write(address + 2, (number >> 8) & 0xFF);
  EEPROM.write(address + 3, number & 0xFF);
}

int readIntFromEEPROM(int address)
{
  return ((int)EEPROM.read(address) << 24) +
         ((int)EEPROM.read(address + 1) << 16) +
         ((int)EEPROM.read(address + 2) << 8) +
         (int)EEPROM.read(address + 3);
}
 
void handleSubmit(){

  Serial.println ("handleSubmit");
  //fetch DMX configuration
    String universeNumStr = server.arg(0);
    String dmxAdrStr = server.arg(1);
  //convert DMX configuration to ints
    universeNum = universeNumStr.toInt();
    dmxAdr = dmxAdrStr.toInt();
  //annnnd write it into EEPROM
    writeIntIntoEEPROM(addr_universeNum, universeNum);
    writeIntIntoEEPROM(addr_dmxAdr, dmxAdr);
    if (EEPROM.commit()) {
      Serial.println("EEPROM successfully committed");
    } else {
      Serial.println("ERROR! EEPROM commit failed");
    }
   
  String
  message = "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  Serial.println(message);
  Serial.println("Universe Number:");
  Serial.println(universeNum);
  Serial.println("DMX Address:");
  Serial.println(dmxAdr);
  server.send ( 200, "text/html", getPage() );
}

//button pressed callback
//ICACHE_RAM_ATTR void buttonPressed(){
//  programMode = !programMode;
//  Serial.println("Mode button pressed!");
//  if (programMode){
//    Serial.println("Address input mode");
//  } else {
//    Serial.println("Operating mode");
//  }
//  Serial.println(" ");
//}

void setup() {
  Serial.begin ( 115200 );
  Serial.println("Setup begin");
  Serial.println(maxUniverses);
  EEPROM.begin(512);
  leds.begin();
  
  artnet.begin();
  artnet.setArtDmxCallback(onDmxFrame);
//  attachInterrupt(digitalPinToInterrupt(BUTTONPIN), buttonPressed, CHANGE);

  //initialize DMX configuration into EEPROM: COMMENT OUT WHEN YOU ARE DONE TESTING!!!!!!!!!!!!
//  writeIntIntoEEPROM(addr_universeNum, 10);
//  writeIntIntoEEPROM(addr_dmxAdr, 11);
//      if (EEPROM.commit()) {
//      Serial.println("EEPROM successfully committed");
//    } else {
//      Serial.println("ERROR! EEPROM commit failed");
//    }

  
  //read eeprom to set global DMX configuration variables
  universeNum = readIntFromEEPROM(addr_universeNum);
  dmxAdr = readIntFromEEPROM(addr_dmxAdr);

  // ACCESS POINT CONFIG
    // -- Initializing the configuration.
    pinMode(BUTTONPIN, INPUT);
  if (digitalRead(BUTTONPIN) == 0){
    Serial.println("Skipping Access Point Setup Mode");
    iotWebConf.skipApStartup();
  } else {
    Serial.println("Entering Access Point Setup Mode");
  }
  iotWebConf.init();
  iotWebConf.setStatusPin(LEDPIN);
  pinMode(LEDPIN, OUTPUT);

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on ( "/submit", handleSubmit );
  server.on("/config", []{ iotWebConf.handleConfig(); });
  server.onNotFound([](){ iotWebConf.handleNotFound(); });

  Serial.println("Ready.");

 initTest();
}

void loop() {
    iotWebConf.doLoop();  
    artnet.read();
}

/**
 * Handle web requests to "/" path.
 */
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = getPage();
  server.send(200, "text/html", s);
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  int universeFromEEPROM = readIntFromEEPROM(addr_universeNum);
  int startAddressFromEEPROM = readIntFromEEPROM(addr_dmxAdr);
  Serial.println("-----FRAME RECIEVED-------");
  Serial.println(universe);

  // read universe and put into the right part of the display buffer
  if (universe == universeFromEEPROM){
    for (int i = 0; i < length / 3; i++)
    {
        leds.setPixelColor(
        i, 
        data[startAddressFromEEPROM + i * 3 - 1], 
        data[startAddressFromEEPROM + i * 3], 
        data[startAddressFromEEPROM + i * 3 + 1]
        );
  //      Serial.println(data[i * 3]);
  //      Serial.println(data[i * 3 + 1]);
  //      Serial.println(data[i * 3 + 2]);
    }
  }   
    leds.show();
}

void initTest()
{
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 127, 0, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 127, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 0, 127);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixelColor(i, 0, 0, 0);
  leds.show();
}
