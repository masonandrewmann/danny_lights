//test
//https://diyprojects.io/esp8266-web-server-tutorial-create-html-interface-connected-object/#.Wyk7dop9jDd

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//#include <Adafruit_BMP280.h>
#include <ESP8266HTTPClient.h>

#define ssid      "ed-law"       // WiFi SSID
#define password  "EDLAWREAB1"        // WiFi password

#define BMP_SCK 13                 
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10

#define LEDPIN D2

float   t = 0 ;
float   h = 0 ;
float   p = 0;
String  LEDstate = "OFF";

int universeNum = 0;
int dmxAdr = 0;

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

void handleRoot(){
  if ( server.hasArg("LED") ) {
    Serial.println("LED");
  } else {
    server.send ( 200, "text/html", getPage() );
  } 
 } 
void handleSubmit(){

  Serial.println ("handleSubmit");
    String universeNumStr = server.arg(0);
    String dmxAdrStr = server.arg(1);

    universeNum = universeNumStr.toInt();
    dmxAdr = dmxAdrStr.toInt();
   
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

void setup() {
  Serial.begin ( 115200 );
  // Init BMP280
//  if ( !bmp.begin() ) {
//    Serial.println("BMP280 KO!");
//    while(1);
//  } else {
//    Serial.println("BMP280 OK");
//  }
 
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
//  t = bmp.readTemperature();
//  p = bmp.readPressure() / 100.0F;
  delay(1000);
}
