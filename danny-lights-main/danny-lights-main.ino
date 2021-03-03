/*
  FastLED + standard 44 Key Ir RGB Led remote + Teensy 3.2
 MMason Mann 3.2.2021
  Adapted from Dean Montgomery --- 44-key IR remote customization
  https://github.com/dmonty2/ir_remote_leds
*/

//from wifi config code
#include <EEPROM.h>
#include "libraries/IotWebConf/src/IotWebConf.h"
//from tmz artnet stuff
#include <ArtnetWifi.h> // https://github.com/natcl/Artnet  
#include <Adafruit_NeoPixel.h>

#include <FastLED.h>

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define IR_TOLERANCE 8

#define LEDPIN 16 //built in
#define BUTTONPIN 13 // D7 on esp8266 dev boad
#define MODEPIN 12 // D6 on esp8266 dev board


///EEPROM STRUCTURE
//0 -- effect number
//1 -- color for solid color
//4 -- color 1 for gradient
//7 -- color 2 for gradient
//10 --- number of bands for randomBands
//13 --- brightness
//16 DIY1 Color
//19 DIY2 Color
//22 DIY3 Color
//25 -- DMX Universe Number
//30 -- DMX Starting Address Number
//35 -- IOT Library Stuff

//EEPROM addresses
int addr_universeNum = 25;
int addr_dmxAdr = 30;

int universeNum;
int dmxAdr;

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "AutoconnectAP";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "password";

DNSServer dnsServer;
WebServer server(80);

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);

int status = WL_IDLE_STATUS;

// Artnet settings
ArtnetWifi artnet;

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
const uint16_t kRecvPin = 14;

IRrecv irrecv(kRecvPin);

decode_results signals;

uint32_t IRCommand;
boolean haveWePickedColors;

//LED setup
#define DATA_PIN    15 //D8
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    60
#define BRIGHTNESS          200   // GLOBAL brightness scale

CRGB leds[NUM_LEDS];
CRGB ledColors[NUM_LEDS];
CHSV color;
CHSV color1;
CHSV color2;
boolean powerOn = true;
enum Rgb { RED, GREEN, BLUE };

//other variables setups

int colorIndex;
#define MIN_intrvl 10  // fastest the led libraries can update without loosing access to PIR remote sensing.
long    intrvl = 40;
unsigned long currentMillis = millis(); // define here so it does not redefine in the loop.
unsigned long previousMillis = 0;
uint8_t effect = 0;
uint8_t h = 0; //hue
uint8_t s = 255; //saturation
uint8_t v = 255; //value
uint8_t brightnessFactor = 20 ; // how many steps to from full bright to black out
uint8_t brightnessInterval = BRIGHTNESS / brightnessFactor ; /// BRIGHTNESS (overall brighntess// max brigtness) divided by btightness factor creates the interval you step by
uint8_t brightness = BRIGHTNESS; // this is new brightness ( BRIGHTNESS - the brightness factor)
uint8_t bands;
boolean initialized = false;

#define SOLIDCOLOR 1
#define GRADIENT 2
#define RANDOMBANDS 3
#define FADE7 4
#define FADE3 5
#define SINELON 6
#define JUGGLE 7
#define BARBERPOLE 8
#define BRIGHT 9
#define DIM 10
#define BARBERPOLE1 11
#define BARBERPOLE2 12
#define EASTEREGGPOLE 13

uint8_t randNumber1;
uint8_t randNumber2;
uint8_t randNumber3;

struct buttonNames {
  uint32_t bright;   uint32_t dim;        uint32_t ppause;     uint32_t power;
  uint32_t red1;     uint32_t green1;     uint32_t blue1;      uint32_t white1;
  uint32_t red2;     uint32_t green2;     uint32_t blue2;      uint32_t white2;
  uint32_t red3;     uint32_t green3;     uint32_t blue3;      uint32_t white3;
  uint32_t red4;     uint32_t green4;     uint32_t blue4;      uint32_t white4;
  uint32_t red5;     uint32_t green5;     uint32_t blue5;      uint32_t white5;
  uint32_t red_up;   uint32_t green_up;   uint32_t blue_up;    uint32_t quick;
  uint32_t red_down; uint32_t green_down; uint32_t blue_down;  uint32_t slow;
  uint32_t diy1;     uint32_t diy2;       uint32_t diy3;       uint32_t autom;
  uint32_t diy4;     uint32_t diy5;       uint32_t diy6;       uint32_t flash;
  uint32_t jump3;    uint32_t jump7;      uint32_t fade3;      uint32_t fade7;
};

//// These are the codes for my remote
const buttonNames remote =  {
  0xFF3AC5,   0xFFBA45,   0xFF827D,   0xFF02FD,
  0xFF1AE5,   0xFF9A65,   0xFFA25D,   0xFF22DD,
  0xFF2AD5,   0xFFAA55,   0xFF926D,   0xFF12ED,
  0xFF0AF5,   0xFF8A75,   0xFFB24D,   0xFF32CD,
  0xFF38C7,   0xFFB847,   0xFF7887,   0xFFF807,
  0xFF18E7,   0xFF9867,   0xFF58A7,   0xFFD827,
  0xFF28D7,   0xFFA857,   0xFF6897,   0xFFE817,
  0xFF08F7,   0xFF8877,   0xFF48B7,   0xFFC837,
  0xFF30CF,   0xFFB04F,   0xFF708F,   0xFFF00F,
  0xFF10EF,   0xFF906F,   0xFF50AF,   0xFFD02F,
  0xFF20DF,   0xFFA05F,   0xFF609F,   0xFFE01F,
};

uint32_t commandsArray[] =  {
  0xFF3AC5,   0xFFBA45,   0xFF827D,   0xFF02FD,
  0xFF1AE5,   0xFF9A65,   0xFFA25D,   0xFF22DD,
  0xFF2AD5,   0xFFAA55,   0xFF926D,   0xFF12ED,
  0xFF0AF5,   0xFF8A75,   0xFFB24D,   0xFF32CD,
  0xFF38C7,   0xFFB847,   0xFF7887,   0xFFF807,
  0xFF18E7,   0xFF9867,   0xFF58A7,   0xFFD827,
  0xFF28D7,   0xFFA857,   0xFF6897,   0xFFE817,
  0xFF08F7,   0xFF8877,   0xFF48B7,   0xFFC837,
  0xFF30CF,   0xFFB04F,   0xFF708F,   0xFFF00F,
  0xFF10EF,   0xFF906F,   0xFF50AF,   0xFFD02F,
  0xFF20DF,   0xFFA05F,   0xFF609F,   0xFFE01F,
};

CHSV r1 (0, 255, 255);
CHSV g1 (96, 255, 255);
CHSV b1 (160, 255, 255);
CHSV w1 (4, 10, 255);
CHSV r2 (16, 255, 255);
CHSV g2 (112, 255, 255);
CHSV b2 (176, 255, 255);
CHSV w2  (52, 200, 255);
CHSV r3 (32, 255, 255);
CHSV g3 (120, 255, 255);
CHSV b3 (192, 255, 255);
CHSV w3 (45, 200, 255);
CHSV r4 (48, 255, 255);
CHSV g4 (128, 255, 255);
CHSV b4 (208, 255, 255);
CHSV w4 (96, 70, 255);
CHSV r5 (60, 255, 255);
CHSV g5 (136, 255, 255);
CHSV b5 (224, 255, 255);
CHSV w5 (64, 60, 255);

// CHSV Color Array

CHSV CHSV_array[] = { r1, g1, b1, w1,
                      r2, g2, b2, w2,
                      r3, g3, b3, w3,
                      r4, g4, b4, w4,
                      r5, g5, b5, w5
                    };

                    // storing diy in eeprom

uint8_t r = 0; //red
uint8_t g = 0; //green
uint8_t b = 0; //blue
#define SLIDE_VALUE 110
#define SLIDE_RGB 111
uint8_t slide = SLIDE_VALUE;
uint8_t last_white = 0;
CHSV water[NUM_LEDS];

struct DIY {
  uint8_t r; uint8_t g; uint8_t b;
};
//Track start points in eeprom index
static uint16_t eeprom_addr[] = {16, 19, 22};

struct DIY_BUTTON {
  uint8_t button;
  bool dirty;
};
DIY_BUTTON lastdiy = { 0, false };
/*
struct DIYSTORE {
  DIY diy1; DIY diy2; DIY diy3; // stored at 0,  3,  6
  DIY diy4; DIY diy5; DIY diy6; // stored at 9, 12, 15
};
 size of EEPROM: 1024 bytes
 size of 44 key remote: 176 bytes
 size of DIY: 3 bytes
 size of DIYSTORE: 18 bytes
 */

void setup() {
  //------------------------SETUP FOR ARTNET MODE------------------------
  Serial.begin ( 115200 );
     while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println("Setup begin");
  EEPROM.begin(512);
  pinMode(MODEPIN, INPUT);
  
  artnet.begin();
  artnet.setArtDmxCallback(onDmxFrame);
  
  //read eeprom to set global DMX configuration variables
  universeNum = readIntFromEEPROM(addr_universeNum);
  dmxAdr = readIntFromEEPROM(addr_dmxAdr);
  Serial.println("Universe Num from EEPROM: ");
  Serial.println(universeNum);
  Serial.println("Starting Address from EEPROM:");
  Serial.println(dmxAdr);
  Serial.println("iot eeprom start ");
  Serial.println(IOTWEBCONF_CONFIG_START);

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

  //------------------------setup for IR remote mode------------------------
  irrecv.enableIRIn();  // Start the receiver
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);
  
//  pinMode(13, OUTPUT);
//  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)

  randomSeed(analogRead(0));
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master BRIGHTNESS control
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();  // clear all pixel data
  FastLED.show();

  startFromEEPROM();
//  EEPROM.get(0, effect); // get that effect number
//  Serial.print("EEPROM loading-- ");
//  Serial.println(effect);
//  
//  if (effect == SOLIDCOLOR) {
//    EEPROM.get(1, color);
//    solidColor(color);
//  }
//  if (effect == GRADIENT) {
//    EEPROM.get(4, color1);
//    EEPROM.get(7, color2);
//    fill_gradient(leds, 0, color1, NUM_LEDS, color2, SHORTEST_HUES);
//    FastLED.show();
//  }
//  if (effect == RANDOMBANDS) {
//    EEPROM.get(10, bands);
//    randomBands(bands);
//  }
}

void loop() {
  int tempMode = digitalRead(MODEPIN); //read switch to move between Art-Net mode and IR mode
  if (tempMode == 1){
  // Art-Net Mode
    iotWebConf.doLoop();  
    artnet.read();
    if (initialized){
      initialized = false;
    }
  } 
  
  else {
    if (!initialized){
      startFromEEPROM();
    }
  currentMillis = millis();
  if (!irrecv.decode(&signals)) {
    if (currentMillis - previousMillis > intrvl) {
      previousMillis = currentMillis;
      update_effect();
      FastLED.show();
    }
  }
  if (irrecv.decode(&signals)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    mainButton();
    serialPrintUint64(signals.value, HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
  delay(100);
  }
}

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
  EEPROM.commit();  
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

void onDmxFrame(uint16_t universe, uint16_t length_, uint8_t sequence, uint8_t* data)
{
  int universeFromEEPROM = readIntFromEEPROM(addr_universeNum);
  int startAddressFromEEPROM = readIntFromEEPROM(addr_dmxAdr);
//  Serial.println("-----FRAME RECIEVED-------");
//  Serial.println(universe);
//  Serial.println(universeFromEEPROM);
//  Serial.println(startAddressFromEEPROM);
//  Serial.println(length_);

    if (universe == universeFromEEPROM){
//      Serial.println("correct Universe");
    for (int i = 0; i < NUM_LEDS; i++)
    {
//      Serial.println(i);
        leds[i].setRGB(
          data[startAddressFromEEPROM + i * 3 - 1],
          data[startAddressFromEEPROM + i * 3],
          data[startAddressFromEEPROM + i * 3 + 1]
          );
    }
  }
//  Serial.println("Done printing frame");   
    FastLED.show();
}

void mainButton() {
//  IRCommand = getButton();
  IRCommand = signals.value;
  if (IRCommand == remote.power) {
//    FastLED.clear();  // clear all pixel data
//    FastLED.show();
//    effect = 0; //ot to reset the effect counter so we dont go into the  effects loop
//    Serial.println("power");
//    startFromEEPROM();
    if (powerOn){
      FastLED.setBrightness(brightness);
    } else {
      FastLED.setBrightness(0);
    }
    powerOn = !powerOn;
  }

  //definitions for just solid colors
  if ( IRCommand == remote.red1 || IRCommand == remote.green1 || IRCommand == remote.blue1 || IRCommand == remote.white1 ||
       IRCommand == remote.red2 || IRCommand == remote.green2 || IRCommand == remote.blue2 || IRCommand == remote.white2 ||
       IRCommand == remote.red3 || IRCommand == remote.green3 || IRCommand == remote.blue3 || IRCommand == remote.white3 ||
       IRCommand == remote.red4 || IRCommand == remote.green4 || IRCommand == remote.blue4 || IRCommand == remote.white4 ||
       IRCommand == remote.red5 || IRCommand == remote.green5 || IRCommand == remote.blue5 || IRCommand == remote.white5 ) {
    for (int i = 4; i < 24; i++) { //the first 4 buttons are not color buttons, so we dont wanna scan through those in the array
//      Serial.println("buttonNum: " + i);
      if (IRCommand == commandsArray[i]) {
        colorIndex = i - 4;
        Serial.print("THE color index is: ");
        Serial.println(i);
      }
    }
    CHSV color =  CHSV_array[colorIndex];
    effect = SOLIDCOLOR;
    solidColor(color);
    //nscale8_video(brightness); // dim to max brightness
    FastLED.show();

    EEPROM.put(0, effect); // Eeprom address 0 is effect/mode
    EEPROM.put(1, color); // eeprom address 1,2,3 are color1
    EEPROM.commit();  
      Serial.println("solid_colors");
  }

  if (IRCommand == remote.autom) {
    FastLED.clear();  // clear all pixel data
    FastLED.show();
    staticGradient();
    IRCommand = 0x00000000;
    haveWePickedColors = false;
    effect = GRADIENT; // got to reset the effect counter so we dont go into the  effects loop
    EEPROM.put(0, effect);
    EEPROM.commit();  
      Serial.println("autom");
  }

  if (IRCommand == remote.jump3) {
    // stage = 1;
    intrvl = 65;
    h = 0; s = 255; v = 0;
    effect = JUGGLE;
    EEPROM.put(0, effect);
    EEPROM.commit();  
      Serial.println("jump3");
  }
  if (IRCommand == remote.jump7) {
    Serial.println("jump 7");
    //  stage = 1;
    intrvl = MIN_intrvl;
    h = 0; s = 255; v = 0;
    effect = SINELON;
    EEPROM.put(0, effect);
    EEPROM.commit();  
  }
  if (IRCommand == remote.fade3) {
    Serial.println("fade 3");
    //stage = 1;
    intrvl = 65;
    h = 0; s = 255; v = 0;
    effect = FADE3;
    EEPROM.put(0, effect);
    EEPROM.commit();  
  }
  if (IRCommand == remote.fade7) {
    intrvl = 65;
    effect = FADE7;
    h = 0; s = 255; v = 255;
    EEPROM.put(0, effect);
    EEPROM.commit();  
      Serial.println("fade 7");
  }
  if (IRCommand == remote.bright) {
    brightness = brightness + brightnessInterval;
    if (brightness > BRIGHTNESS) {
      brightness = BRIGHTNESS;
    }
    FastLED.setBrightness(brightness);
    FastLED.show();
    Serial.print("brightness pressed. brightness = ");
    Serial.println(brightness);
    EEPROM.put(13, brightness);
    EEPROM.commit();  
  }

  if (IRCommand == remote.dim) {
    brightness = brightness - brightnessInterval;
    if (brightness > BRIGHTNESS) {
      brightness = 0;
    }
    FastLED.setBrightness(brightness);
    FastLED.show();
    Serial.print("dimmer pressed. brightness = ");
    Serial.println(brightness);
    EEPROM.put(13, brightness);
    EEPROM.commit();  
  }
  if (IRCommand == remote.slow) {
    if (intrvl >= 100) {
      intrvl += 100;
    } else {
      intrvl += 15;
    }
    Serial.print("slow pressed. interval = ");
    Serial.println(intrvl);
  }
  if (IRCommand == remote.quick) {
    if ( intrvl >= 101 ) {
      intrvl -= 100;
    } else if ( intrvl >= MIN_intrvl + 10 && intrvl <= 100 ) {
      intrvl -= 10;
    } else {
      intrvl = MIN_intrvl;
    }
    Serial.print("quick pressed. interval = ");
    Serial.println(intrvl);
  }
  if (IRCommand == remote.diy1) {
    Serial.println("diy1");
    updateDiy(0);
  }
  if (IRCommand == remote.diy2) {
    Serial.println("diy2");
    updateDiy(1);
  }
  if (IRCommand == remote.diy3) {
    Serial.println("diy3");
    updateDiy(2);
  }
  if (IRCommand == remote.diy4) {
    effect = RANDOMBANDS;
    bands = 2;
    randomBands(bands);
    EEPROM.put(0, effect);
    EEPROM.put(10, bands);
    EEPROM.commit();  
    Serial.println("diy4");
  }
  if (IRCommand == remote.diy5) {
    effect = RANDOMBANDS;
    bands = 3;
    randomBands(bands);
    EEPROM.put(0, effect);
    EEPROM.put(10, bands);
    EEPROM.commit();  
    Serial.println("diy5");
  }
  if (IRCommand == remote.diy6) {
    effect = RANDOMBANDS;
    bands = 4;
    randomBands(bands);
    EEPROM.put(0, effect);
    EEPROM.put(10, bands);
    EEPROM.commit();  
    Serial.println("diy6");
  }
  if (IRCommand == remote.red_up) {
    colorUpDown(RED, 10); 
  }
  if (IRCommand == remote.green_up) {
    colorUpDown(GREEN, 10); 
  }
  if (IRCommand == remote.blue_up) {
    colorUpDown(BLUE, 10); 
  }
  if (IRCommand == remote.red_down) {
    colorUpDown(RED, -10); 
  }
  if (IRCommand == remote.green_down) {
    colorUpDown(GREEN, -10); 
  }
  if (IRCommand == remote.blue_down) {
    colorUpDown(BLUE, -10); 
  }
  if (IRCommand == remote.flash) {
    intrvl = 0;
    if (effect == BARBERPOLE) {
      effect = BARBERPOLE1;
    } else if (effect == BARBERPOLE1) {
      effect = BARBERPOLE2;
    } else if (effect == BARBERPOLE2) {
      effect = EASTEREGGPOLE;
    } else {
      effect = BARBERPOLE;
    }
    Serial.println("flash");
  }
//  if (IRCommand != remote.red_up && IRCommand != remote.green_up && IRCommand != remote.blue_up &&
//      IRCommand != remote.red_down && IRCommand != remote.green_down && IRCommand != remote.blue_down && lastdiy.dirty){
//        lastdiy.dirty = false;
//      }

//check if we need to exit diy loop
  boolean realButton = false; //check if the ir code corresponds to a button
    for (int i = 0; i < 44; i++) { //the first 4 buttons are not color buttons, so we dont wanna scan through those in the array
//      Serial.println("buttonNum: " + i);
      if (IRCommand == commandsArray[i] && IRCommand != remote.red_up && IRCommand != remote.green_up && IRCommand != remote.blue_up &&
        IRCommand != remote.red_down && IRCommand != remote.green_down && IRCommand != remote.blue_down && lastdiy.dirty) {
        Serial.println("exiting diy mode");
        lastdiy.dirty = false;
        lastdiy.button = 0;
      }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////END OF GET BUTTON FUNC ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void solidColor(CHSV color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
}

void staticGradient() {
  Serial.println("in static gradient");
  while (!haveWePickedColors) { //defaults to false
    CHSV color1 = getButtonColorNew();
//    Serial.println("COLOR 1 FETCHED YAYY");
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = color1;
    }
    FastLED.show();
    Serial.println("color1");
    IRCommand = 0xFFF00F; // hacky solution because the while loop in getButtonColorNew is weird
    CHSV color2 = getButtonColorNew();
    fill_gradient(leds, 0, color1, NUM_LEDS, color2, SHORTEST_HUES);
    EEPROM.put(4, color1); // address 4,5,6 are first color
    EEPROM.put(7, color2); // address 7,8,9 are second color
    EEPROM.commit();  
    FastLED.show();

    haveWePickedColors = true;
  }
}

CHSV getButtonColor() { // need to wait until we get a color here
  int colorIndex = 99;
  while (colorIndex == 99) {
    IRCommand = getButton();     // if IR signals is received, then do this
    while (IRCommand == 0) {
      IRCommand = getButton();
      //Serial.println("in this color 1 while loop");
    }
    Serial.print("in get color1: ");
    Serial.println(IRCommand, HEX);
    IRCommand = 99;
    for (int i = 4; i < 24; i++) {
      Serial.print(i);
      if (IRCommand == commandsArray[i]) {
        colorIndex = i - 4;
        Serial.print("the SUCESSFUL color index for gradient is: ");
        Serial.println(i);
      }

      if (i == 23 && colorIndex == 99) { // check to see if we have a weird non button code so we dont get stuck here
        //getButtonColor();
        Serial.print("button pressed that was not a color. ColorIndex is: ");
        Serial.println(colorIndex);
      }
    }
  }
  return CHSV_array[colorIndex];
}

CHSV getButtonColorNew(){
  Serial.println(IRCommand, HEX);
  int colorIndex = 99;
  while (colorIndex == 99) {
    while (IRCommand == 0xFFF00F) {
      if (irrecv.decode(&signals)){
        IRCommand = getButtonNew();
      }
      Serial.println("in this color 1 while loop");
      delay(10);
    }
    Serial.print("in get color1: ");
    Serial.println(IRCommand, HEX);
//    colorIndex = 99;
    for (int i = 4; i < 24; i++) {
      Serial.print(i);
      if (IRCommand == commandsArray[i]) {
        colorIndex = i - 4;
        Serial.print("the SUCESSFUL color index for gradient is: ");
        Serial.println(i);
      }

      if (i == 23 && colorIndex == 99) { // check to see if we have a weird non button code so we dont get stuck here
        //getButtonColor();
        IRCommand = 0xFFF00F;
        Serial.print("button pressed that was not a color. ColorIndex is: ");
        Serial.println(colorIndex);
      }
    }
  }
  return CHSV_array[colorIndex];
}


uint32_t getButton() {
  if (irrecv.decode(&signals))
  //Serial.println(signals, HEX);
  {

    uint64_t irVal = 0;
    for (int i = 3; i < (signals.rawlen - 1); i++)
    {
      int Delta = signals.rawbuf[i] - signals.rawbuf[i + 1];
      Serial.println(Delta, HEX);
      if (Delta < 0) Delta = -Delta;
      uint8_t b = (Delta < IR_TOLERANCE) ? 0 : 1;

      if ((i - 3) < 63)
      {
        irVal = irVal | ((int64_t)b << (int64_t)(i - 3));
      }
    }

    uint64_t x1 = irVal;
    uint32_t x = x1 >> 32;
    //    x = x1 & 0xFFFFFFFF;
    //    Serial.println(x, HEX);
    IRCommand = x;
    Serial.print("GetButton IrCommand: ");
    Serial.println(IRCommand, HEX);
    irrecv.resume(); // get the next signal
    return IRCommand;
  }
}

uint32_t getButtonNew(){
//  irrecv.resume();
  if (irrecv.decode(&signals)){
      Serial.println("IR signal received");
      IRCommand = signals.value;
      irrecv.resume(); // get the next signal
      return IRCommand;
  }
}

void update_effect() {
  if ( effect == FADE7 ) {
    cyclingRainbow();
  } else if ( effect == JUGGLE ) {
    juggle();
  } else if ( effect == SINELON ) {
    sinelon();
  } else if ( effect == FADE3 ) {
    allRainbow();
  } else if (effect == BARBERPOLE) {
    barberPole();
  } else if (effect == BARBERPOLE1) {
    barberPole();
  } else if (effect == BARBERPOLE2) {
    barberPole();
  } else if (effect == EASTEREGGPOLE) {
    easterEggPole();
    effect = BARBERPOLE;
  }
}

void cyclingRainbow() {          //cycling rainbow
  for ( int i = 0; i < NUM_LEDS; i++ ) {
    leds[i] = CHSV(h + i, s, 255);
  }
  h = h + 10;
}

void allRainbow() {        // all colors rainbow
  for ( int i = 0; i < NUM_LEDS; i++ ) {
    leds[i] = CHSV(h, s, 255);
  }
  h = h + 10;
}


void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 16);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue, 255, 255); //Adjusted Brightness with variable
    dothue += 32;
  }
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 16);
  int pos = beatsin16(6, 0, NUM_LEDS);
  leds[pos] += CHSV( h, 255, 255);
  h++;
}

void randomBands(int numBands) {
  int randomNumber[numBands] ; // create and array that is the total number of bands long. if we want 4 bands, we have to have 4 random colors.
  int randomNumber2[numBands] ; // create and array that is the total number of bands long. if we want 4 bands, we have to have 4 random colors.
  for (int i = 0; i < numBands; i++) {
    randomNumber[i] = random(255);
    randomNumber2[i] = random(200, 255);
  }
  for (int i = 0; i <= NUM_LEDS / numBands; i++) {
    leds[i] = CHSV(randomNumber[0], randomNumber2[0], 255);
  }

  if (numBands <= 6) {      //random colors for bands less then six
    Serial.println("in bands less then 6");
    for (int j = 1; j < numBands; j++) {
      for (int i = ((NUM_LEDS / numBands) * j) + 1; i <= (NUM_LEDS / numBands) * (j + 1) ; i++) {
        leds[i] = CHSV(randomNumber[j], randomNumber2[j], 255);
      }
    }
  }
  else if (numBands <= 30) {  //more then 6 bands, we repeat 3 colors
    Serial.println("in bands less then 30");

    for (int j = 1; j < numBands; j++) {
      for (int i = ((NUM_LEDS / numBands) * j) + 1; i <= (NUM_LEDS / numBands) * (j + 1) ; i++) {
        leds[i] = CHSV(randomNumber[j % 3], randomNumber2[j], 255);
      }
    }
  }  else if (numBands > 30) {  // full saturation when they get close to each other
    Serial.println("in bands greater then 30");

    for (int j = 1; j <= numBands; j++) {
      for (int i = ((NUM_LEDS / numBands) * j) + 1; i <= (NUM_LEDS / numBands) * (j + 1) ; i++) {
        leds[i] = CHSV(randomNumber[j % 3], 255, 255);
      }
    }
  }
  FastLED.show();
}

// scrolling the colors down the line
void barberPole() {
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = leds[(i + 1) % NUM_LEDS];
  }
}

void easterEggPole() { // a secret effect if you press FLASH 3 times
  int numBands = 12;
  CRGB barberPoleColors[4] = {CRGB::Red, CRGB::White, CRGB::Blue, CRGB::White} ; // create an  array for the 3 colors
  //  for (int i = 0; i < numBands; i++) {
  //    randomNumber[i] = random(255);
  //    randomNumber2[i] = random(200, 255);
  //  }
  for (int i = 0; i <= NUM_LEDS / numBands; i++) {
    leds[i] = barberPoleColors[0];
  }
  for (int j = 1; j < numBands; j++) {
    for (int i = ((NUM_LEDS / numBands) * j) + 1; i <= (NUM_LEDS / numBands) * (j + 1) ; i++) {
      leds[i] =  barberPoleColors[j % 4];
    }
  }
}

void startFromEEPROM(){
  int effNum = (int)EEPROM.read(0);
  initialized = true;

  EEPROM.get(13, brightness);
  Serial.println(brightness); 
    FastLED.setBrightness(brightness);

  Serial.println(effNum);
  
  switch (effNum) {
    case 1:
      Serial.println("solid color mode");
//      color = readIntFromEEPROM(1);
      EEPROM.get(1, color);
      solidColor(color);
    break;

    case 2: 
       Serial.println("gradient mode");
       EEPROM.get(4, color1);
       EEPROM.get(7, color2);
      fill_gradient(leds, 0, color1, NUM_LEDS, color2, SHORTEST_HUES);
      FastLED.show();
    break;

    case RANDOMBANDS:
    EEPROM.get(10, bands);
    randomBands(bands);
    break;
    
    default:
      Serial.println("an animiation probably");
    break;
}
}

void updateDiy(uint8_t num){
  effect = 0;
  Serial.print("diy ");
  Serial.println(num);
  Serial.println(eeprom_addr[num]);
  if ( lastdiy.button == num && lastdiy.dirty == true){
    Serial.println("storing DIY");
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
    // avoid 100,000 write cycle limit by only writing what is needed.
    EEPROM.write(eeprom_addr[num], r);
    EEPROM.write(eeprom_addr[num] + 1, g);
    EEPROM.write(eeprom_addr[num] + 2, b);
    lastdiy.dirty = false;
  } else {
    lastdiy.button = num;
    lastdiy.dirty = false;
    DIY diy;
    EEPROM.get(eeprom_addr[num], diy);
    // default was 255 so make unset value a dull grey 
//    if ( diy.r > BRIGHTNESS ) diy.r = BRIGHTNESS/3;
//    if ( diy.g > BRIGHTNESS ) diy.g = BRIGHTNESS/3;
//    if ( diy.b > BRIGHTNESS ) diy.b = BRIGHTNESS/3;
//    r = EEPROM.read(eeprom_addr[num]);
//    g = EEPROM.read(eeprom_addr[num] + 1);
//    b = EEPROM.read(eeprom_addr[num] + 2);
    r = diy.r;
    g = diy.g;
    b = diy.b;
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
    fill_solid(leds, NUM_LEDS, CRGB( r, g, b ));
  }
  slide = SLIDE_RGB;
  FastLED.show();
}

void colorUpDown(int color, int8_t val){
  effect = 0;
  r = leds[1].r; g = leds[1].g; b = leds[1].b;
  uint8_t z[] = { r, g, b };
  if ( z[color] + val >= 1 && z[color] + val <= 250 ){
    if ( color == RED   ) { r += val; }
    if ( color == GREEN ) { g += val; }
    if ( color == BLUE  ) { b += val; }
    lastdiy.dirty = true;
    fill_solid(leds, NUM_LEDS, CRGB( r, g, b ));
  }
  slide = SLIDE_RGB;
  FastLED.show();
  //Serial.print("updown ");
  //Serial.print(color);
  //Serial.print(" ");
  //Serial.println(val);
}
