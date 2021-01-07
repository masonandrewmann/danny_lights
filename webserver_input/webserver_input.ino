//test
//https://diyprojects.io/esp8266-web-server-tutorial-create-html-interface-connected-object/#.Wyk7dop9jDd

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <IotWebConf.h>
//#include <Adafruit_BMP280.h>
#include <ESP8266HTTPClient.h>

#define ssid      "ed-law"       // WiFi SSID
#define password  "EDLAWREAB1"        // WiFi password

#define BMP_SCK 13                 
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

#define LEDPIN 16
#define BUTTONPIN 13 // (D7)
#define IOTWEBCONF_CONFIG_START 8

//EEPROM addresses
int addr_universeNum = 0;
int addr_dmxAdr = 4;

int universeNum;
int dmxAdr;

//
boolean programMode = false;

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "AutoconnectAP";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "password";

DNSServer dnsServer;
WebServer server(80);

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);


//create Objects
//Adafruit_BMP280 bmp; // I2C
//ESP8266WebServer server ( 80 );

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

//void handleRoot(){
//  if ( server.hasArg("LED") ) {
//    Serial.println("LED");
//  } else {
//    server.send ( 200, "text/html", getPage() );
//  } 
// } 
 
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

ICACHE_RAM_ATTR void buttonPressed(){
  programMode = !programMode;
  Serial.println("Mode button pressed!");
  if (programMode){
    Serial.println("Address input mode");
  } else {
    Serial.println("Operating mode");
  }
  Serial.println(" ");
}

void setup() {
  Serial.begin ( 115200 );
  Serial.println("Setup begin");
  EEPROM.begin(512);
  
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

 
}

void loop() {
    iotWebConf.doLoop();  
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
