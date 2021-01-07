//test
//https://diyprojects.io/esp8266-web-server-tutorial-create-html-interface-connected-object/#.Wyk7dop9jDd

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
//#include <Adafruit_BMP280.h>
#include <ESP8266HTTPClient.h>

#define ssid      "ed-law"       // WiFi SSID
#define password  "EDLAWREAB1"        // WiFi password

#define BMP_SCK 13                 
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

#define LEDPIN D2
#define BUTTONPIN 15

//EEPROM addresses
int addr_universeNum = 0;
int addr_dmxAdr = 5;

int universeNum;
int dmxAdr;

//
boolean programMode = false;

//create Objects
//Adafruit_BMP280 bmp; // I2C
ESP8266WebServer server ( 80 );

String getPage(){
  //String page = "<html lang=fr-FR><head><meta http-equiv='refresh' content='10'/>";
  String page = "<html lang=fr-FR><head>";
  page += "<title>Light Bar</title>";
  page += "<style> body { background-color: #fffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  page += "</head><body><h1>DMX Configuration</h1>";
  page += "<h3>Universe</h3>";
  page += "<form action='/submit' method='POST'>";
  page += "<ul><li><input type='number' name='Uni' id='Uni' value='" + String(universeNum) + "' min='0' max='24'>";
  page += "</li></ul>";
  page += "<br>";
  page += "<h3>Address</h3>";
  page += "<form action='submit' method='POST'>";
  page += "<ul><li><input type='number' name='Adr' id = 'Adr' value ='" + String(dmxAdr) + "' min='0' max='24'>";
  page += "<br><br><br>";
  page += "<input type = 'submit' value='Submit'></li></ul>";
  page += "</body></html>";
  return page;
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

void handleRoot(){
  if ( server.hasArg("LED") ) {
    Serial.println("LED");
  } else {
    server.send ( 200, "text/html", getPage() );
  } 
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
  
  attachInterrupt(digitalPinToInterrupt(BUTTONPIN), buttonPressed, RISING);
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
  
  WiFi.begin ( ssid, password );
  // Waiting for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 ); Serial.print ( "." );
  }
  //WiFi connection is OK
  Serial.println ( "" );
  Serial.print ( "Connected to " ); Serial.println ( ssid );
  Serial.print ( "IP address: " ); Serial.println ( WiFi.localIP() );

  // Link to the function that manage launch page
  server.on ( "/", handleRoot );
  server.on ( "/submit", handleSubmit );

  server.begin();
  Serial.println ( "HTTP server started" );
 
}

void loop() {
  server.handleClient();
  delay(1000);
}
