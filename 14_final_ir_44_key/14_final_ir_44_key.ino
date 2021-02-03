/*
  FastLED + standard 44 Key Ir RGB Led remote + Teensy 3.2
  William Kennedy 5.21.2020
  Adapted from Dean Montgomery --- 44-key IR remote customization
  https://github.com/dmonty2/ir_remote_leds
*/
//TODO figure out better way to slow down that is not so laggy jumpy.
// maybe do play pause button. really it would only be for the Juggle effect cause that is looking so cool.

#include <EEPROM.h>
//#include <IRremote.h>
#include <FastLED.h>

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
// Note: GPIO 16 won't work on the ESP8266 as it does not have interrupts.
const uint16_t kRecvPin = 14;

IRrecv irrecv(kRecvPin);

decode_results results;

uint32_t IRCommand;
boolean haveWePickedColors;

//LED setup
#define DATA_PIN    15 //D5
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    149
#define BRIGHTNESS          200   // GLOBAL brightness scale

CRGB leds[NUM_LEDS];
CRGB ledColors[NUM_LEDS];
CHSV color;
CHSV color1;
CHSV color2;

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

///EEPROM STRUCTURE
//0 -- effect number
//1 -- color for solid color
//4 -- color 1 for gradient
//7 -- color 2 for gradient
//10 --- number of bands for randomBands

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
  0x4C0F33F0,   0x4C0C33F3,   0x4FFC3003,   0x4FFF3000,
  0x4C3F33C0,   0x4C3C33C3,   0x4FCC3033,   0x4FCF3030,
  0x4CCF3330,   0x4CCC3333,   0x4F3C30C3,   0x4F3F30C0,
  0x4CFF3300,   0x4CFC3303,   0x4F0C30F3,   0x4F0F30F0,
  0x7C0F03F0,   0x7C0C03F3,   0x7C0303FC,   0x7C0003FF,
  0x7C3F03C0,   0x7C3C03C3,   0x7C3303CC,   0x7C3003CF,
  0x7CCF0330,   0x7CCC0333,   0x7CC3033C,   0x7CC0033F,
  0x7CFF0300,   0x7CFC0303,   0x7CF3030C,   0x7CF0030F,
  0x7F0F00F0,   0x7F0C00F3,   0x7F0300FC,   0x7F0000FF,
  0x7F3F00C0,   0x7F3C00C3,   0x7F3300CC,   0x7F3000CF,
  0x7FCF0030,   0x7FCC0033,   0x7FC3003C,   0x7FC0003F,
};

uint32_t commandsArray[] =  {
  0x4C0F33F0,   0x4C0C33F3,   0x4FFC3003,   0x4FFF3000,
  0x4C3F33C0,   0x4C3C33C3,   0x4FCC3033,   0x4FCF3030,
  0x4CCF3330,   0x4CCC3333,   0x4F3C30C3,   0x4F3F30C0,
  0x4CFF3300,   0x4CFC3303,   0x4F0C30F3,   0x4F0F30F0,
  0x7C0F03F0,   0x7C0C03F3,   0x7C0303FC,   0x7C0003FF,
  0x7C3F03C0,   0x7C3C03C3,   0x7C3303CC,   0x7C3003CF,
  0x7CCF0330,   0x7CCC0333,   0x7CC3033C,   0x7CC0033F,
  0x7CFF0300,   0x7CFC0303,   0x7CF3030C,   0x7CF0030F,
  0x7F0F00F0,   0x7F0C00F3,   0x7F0300FC,   0x7F0000FF,
  0x7F3F00C0,   0x7F3C00C3,   0x7F3300CC,   0x7F3000CF,
  0x7FCF0030,   0x7FCC0033,   0x7FC3003C,   0x7FC0003F,
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

void setup()
{ Serial.begin(115200);
  irrecv.enableIRIn();  // Start the receiver
    while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(kRecvPin);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)

//  Serial.begin(9600);
//  randomSeed(analogRead(0));
//  // In case the interrupt driver crashes on setup, give a clue
//  // to the user what's going on.
//  Serial.println("Enabling IRin");
//  irrecv.enableIRIn(); // Start the receiver
//  Serial.println("Enabled IRin");
//  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
//  // set master BRIGHTNESS control
//  FastLED.setBrightness(BRIGHTNESS);
//  FastLED.clear();  // clear all pixel data
//  FastLED.show();
//  EEPROM.get(0, effect); // get that effect number
//  Serial.print("EEPROM loading-- ");
//  Serial.println(effect);
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
//  currentMillis = millis();
//  mainButton();
//  if (!irrecv.decode(&signals)) {
//    //Serial.println("we are in the effect looper");
//    if (currentMillis - previousMillis > intrvl) {
//      previousMillis = currentMillis;
//      if (effect > 3) {
//        update_effect();
//        FastLED.show();
//      }
//    }
//  }
  if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
  }
  delay(100);
}
//
//void mainButton() {
////  IRCommand = getButton();
//
//  if (IRCommand == remote.power) {
//    FastLED.clear();  // clear all pixel data
//    FastLED.show();
//    effect = 0; //ot to reset the effect counter so we dont go into the  effects loop
//  }
//
//  //definitions for just solid colors
//  if ( IRCommand == remote.red1 || IRCommand == remote.green1 || IRCommand == remote.blue1 || IRCommand == remote.white1 ||
//       IRCommand == remote.red2 || IRCommand == remote.green2 || IRCommand == remote.blue2 || IRCommand == remote.white2 ||
//       IRCommand == remote.red3 || IRCommand == remote.green3 || IRCommand == remote.blue3 || IRCommand == remote.white3 ||
//       IRCommand == remote.red4 || IRCommand == remote.green4 || IRCommand == remote.blue4 || IRCommand == remote.white4 ||
//       IRCommand == remote.red5 || IRCommand == remote.green5 || IRCommand == remote.blue5 || IRCommand == remote.white5 ) {
//    for (int i = 4; i < 24; i++) { //the first 4 buttons are not color buttons, so we dont wanna scan through those in the array
//      Serial.print(i);
//      if (IRCommand == commandsArray[i]) {
//        colorIndex = i - 4;
//        Serial.print("THE color index is: ");
//        Serial.println(i);
//      }
//    }
//    CHSV color =  CHSV_array[colorIndex];
//    effect = SOLIDCOLOR;
//    solidColor(color);
//    //nscale8_video(brightness); // dim to max brightness
//    FastLED.show();
//
//    EEPROM.put(0, effect); // Eeprom address 0 is effect/mode
//    EEPROM.put(1, color); // eeprom address 1,2,3 are color1
//  }
//
//  if (IRCommand == remote.autom) {
//    FastLED.clear();  // clear all pixel data
//    FastLED.show();
//    staticGradient();
//    IRCommand = 0x00000000;
//    haveWePickedColors = false;
//    effect = GRADIENT; // got to reset the effect counter so we dont go into the  effects loop
//    EEPROM.put(0, effect);
//  }
//
//  if (IRCommand == remote.jump3) {
//    // stage = 1;
//    intrvl = 65;
//    h = 0; s = 255; v = 0;
//    effect = JUGGLE;
//    EEPROM.put(0, effect);
//  }
//  if (IRCommand == remote.jump7) {
//    //Serial.println("jump 7");
//    //  stage = 1;
//    intrvl = MIN_intrvl;
//    h = 0; s = 255; v = 0;
//    effect = SINELON;
//    EEPROM.put(0, effect);
//  }
//  if (IRCommand == remote.fade3) {
//    //Serial.println("fade 3");
//    //stage = 1;
//    intrvl = 65;
//    h = 0; s = 255; v = 0;
//    effect = FADE3;
//    EEPROM.put(0, effect);
//  }
//  if (IRCommand == remote.fade7) {
//    intrvl = 65;
//    effect = FADE7;
//    h = 0; s = 255; v = 255;
//    EEPROM.put(0, effect);
//  }
//  if (IRCommand == remote.bright) {
//    brightness = brightness + brightnessInterval;
//    if (brightness > BRIGHTNESS) {
//      brightness = BRIGHTNESS;
//    }
//    FastLED.setBrightness(brightness);
//    FastLED.show();
//    Serial.print("brightness pressed. brightness = ");
//    Serial.println(brightness);
//  }
//
//  if (IRCommand == remote.dim) {
//    brightness = brightness - brightnessInterval;
//    if (brightness > BRIGHTNESS) {
//      brightness = 0;
//    }
//    FastLED.setBrightness(brightness);
//    FastLED.show();
//    Serial.print("dimmer pressed. brightness = ");
//    Serial.println(brightness);
//  }
//  if (IRCommand == remote.slow) {
//    if (intrvl >= 100) {
//      intrvl += 100;
//    } else {
//      intrvl += 15;
//    }
//    Serial.print("slow pressed. interval = ");
//    Serial.println(intrvl);
//  }
//  if (IRCommand == remote.quick) {
//    if ( intrvl >= 101 ) {
//      intrvl -= 100;
//    } else if ( intrvl >= MIN_intrvl + 10 && intrvl <= 100 ) {
//      intrvl -= 10;
//    } else {
//      intrvl = MIN_intrvl;
//    }
//    Serial.print("quick pressed. interval = ");
//    Serial.println(intrvl);
//  }
//  if (IRCommand == remote.diy1) {
//    effect = RANDOMBANDS;
//    bands = 1;
//    randomBands(bands);
//    EEPROM.put(0, effect);
//    EEPROM.put(10, bands);
//
//  }
//  if (IRCommand == remote.diy2) {
//    effect = RANDOMBANDS;
//    bands = 2;
//    randomBands(bands);
//    EEPROM.put(0, effect);
//    EEPROM.put(10, bands);
//  }
//  if (IRCommand == remote.diy3) {
//    effect = RANDOMBANDS;
//    bands = 3;
//    randomBands(bands);
//    EEPROM.put(0, effect);
//    EEPROM.put(10, bands);
//  }
//  if (IRCommand == remote.diy4) {
//    effect = RANDOMBANDS;
//    bands = 4;
//    randomBands(bands);
//    EEPROM.put(0, effect);
//    EEPROM.put(10, bands);
//  }
//  if (IRCommand == remote.diy5) {
//    effect = RANDOMBANDS;
//    bands = 5;
//    randomBands(bands);
//    EEPROM.put(0, effect);
//    EEPROM.put(10, bands);
//  }
//  if (IRCommand == remote.diy6) {
//    effect = RANDOMBANDS;
//    bands = 6;
//    randomBands(bands);
//    EEPROM.put(0, effect);
//    EEPROM.put(10, bands);
//  }
//  if (IRCommand == remote.red_up) {
//    effect = RANDOMBANDS;
//    bands = 9;
//    randomBands(bands);
//    EEPROM.put(0, effect);
//    EEPROM.put(10, bands);
//  }
//  if (IRCommand == remote.green_up) {
//    effect = RANDOMBANDS;
//    bands = 12;
//    randomBands(bands);
//    EEPROM.put(0, effect);
//    EEPROM.put(10, bands);
//  }
//  if (IRCommand == remote.blue_up) {
//    effect = RANDOMBANDS;
//    bands = 15;
//    randomBands(bands);
//    EEPROM.put(0, effect);
//    EEPROM.put(10, bands);
//  }
//  if (IRCommand == remote.red_down) {
//    effect = RANDOMBANDS;
//    bands = 30;
//    randomBands(bands);
//    EEPROM.put(0, effect);
//    EEPROM.put(10, bands);
//  }
//  if (IRCommand == remote.green_down) {
//    effect = RANDOMBANDS;
//    bands = 42;
//    randomBands(bands);
//    EEPROM.put(0, effect);
//    EEPROM.put(10, bands);
//  }
//  if (IRCommand == remote.blue_down) {
//    effect = RANDOMBANDS;
//    bands = 60;
//    randomBands(bands);
//    EEPROM.put(0, effect);
//    EEPROM.put(10, bands);
//  }
//  if (IRCommand == remote.flash) {
//    intrvl = 0;
//    if (effect == BARBERPOLE) {
//      effect = BARBERPOLE1;
//    } else if (effect == BARBERPOLE1) {
//      effect = BARBERPOLE2;
//    } else if (effect == BARBERPOLE2) {
//      effect = EASTEREGGPOLE;
//    } else {
//      effect = BARBERPOLE;
//    }
//  }
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////END OF GET BUTTON FUNC ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//
//void solidColor(CHSV color) {
//  for (int i = 0; i < NUM_LEDS; i++) {
//    leds[i] = color;
//  }
//  FastLED.show();
//}
//
//void staticGradient() {
//  Serial.println("in static gradient");
//  while (!haveWePickedColors) { //defaults to false
//    CHSV color1 = getButtonColor();
//    for (int i = 0; i < NUM_LEDS; i++) {
//      leds[i] = color1;
//    }
//    FastLED.show();
//    Serial.println("color1");
//    CHSV color2 = getButtonColor();
//    fill_gradient(leds, 0, color1, NUM_LEDS, color2, SHORTEST_HUES);
//    EEPROM.put(4, color1); // address 4,5,6 are first color
//    EEPROM.put(7, color2); // address 7,8,9 are second color
//    FastLED.show();
//
//    haveWePickedColors = true;
//  }
//}
//
////CHSV getButtonColor() { // need to wait until we get a color here
////  int colorIndex = 99;
////  while (colorIndex == 99) {
////    IRCommand = getButton();     // if IR signals is received, then do this
////    while (IRCommand == 0) {
////      IRCommand = getButton();
////      //Serial.println("in this color 1 while loop");
////    }
////    Serial.print("in get color1: ");
////    Serial.println(IRCommand, HEX);
////    colorIndex = 99;
////    for (int i = 4; i < 24; i++) {
////      Serial.print(i);
////      if (IRCommand == commandsArray[i]) {
////        colorIndex = i - 4;
////        Serial.print("the SUCESSFUL color index for gradient is: ");
////        Serial.println(i);
////      }
////
////      if (i == 23 && colorIndex == 99) { // check to see if we have a weird non button code so we dont get stuck here
////        //getButtonColor();
////        Serial.print("button pressed that was not a color. ColorIndex is: ");
////        Serial.println(colorIndex);
////      }
////    }
////  }
////  return CHSV_array[colorIndex];
////}
//
//
//
////uint32_t getButton() {
////  if (irrecv.decode(&signals))
////  //Serial.println(signals, HEX);
////  {
////
////    uint64_t irVal = 0;
////    for (int i = 3; i < (signals.rawlen - 1); i++)
////    {
////      int Delta = signals.rawbuf[i] - signals.rawbuf[i + 1];
////      Serial.println(Delta, HEX);
////      if (Delta < 0) Delta = -Delta;
////      uint8_t b = (Delta < IR_TOLERANCE) ? 0 : 1;
////
////      if ((i - 3) < 63)
////      {
////        irVal = irVal | ((int64_t)b << (int64_t)(i - 3));
////      }
////    }
////
////    uint64_t x1 = irVal;
////    uint32_t x = x1 >> 32;
////    //    x = x1 & 0xFFFFFFFF;
////    //    Serial.println(x, HEX);
////    IRCommand = x;
////    Serial.print("GetButton IrCommand: ");
////    Serial.println(IRCommand, HEX);
////    irrecv.resume(); // get the next signal
////    return IRCommand;
////  }
////}
//
//void update_effect() {
//  if ( effect == FADE7 ) {
//    cyclingRainbow();
//  } else if ( effect == JUGGLE ) {
//    juggle();
//  } else if ( effect == SINELON ) {
//    sinelon();
//  } else if ( effect == FADE3 ) {
//    allRainbow();
//  } else if (effect == BARBERPOLE) {
//    barberPole();
//  } else if (effect == BARBERPOLE1) {
//    barberPole();
//  } else if (effect == BARBERPOLE2) {
//    barberPole();
//  } else if (effect == EASTEREGGPOLE) {
//    easterEggPole();
//    effect = BARBERPOLE;
//  }
//}
//
//void cyclingRainbow() {          //cycling rainbow
//  for ( int i = 0; i < NUM_LEDS; i++ ) {
//    leds[i] = CHSV(h + i, s, 255);
//  }
//  h = h + 10;
//}
//
//void allRainbow() {        // all colors rainbow
//  for ( int i = 0; i < NUM_LEDS; i++ ) {
//    leds[i] = CHSV(h, s, 255);
//  }
//  h = h + 10;
//}
//
//
//void juggle() {
//  // eight colored dots, weaving in and out of sync with each other
//  fadeToBlackBy( leds, NUM_LEDS, 16);
//  byte dothue = 0;
//  for ( int i = 0; i < 8; i++) {
//    leds[beatsin16(i + 7, 0, NUM_LEDS)] |= CHSV(dothue, 255, 255); //Adjusted Brightness with variable
//    dothue += 32;
//  }
//}
//
//void sinelon()
//{
//  // a colored dot sweeping back and forth, with fading trails
//  fadeToBlackBy( leds, NUM_LEDS, 16);
//  int pos = beatsin16(6, 0, NUM_LEDS);
//  leds[pos] += CHSV( h, 255, 255);
//  h++;
//}
//
//void randomBands(int numBands) {
//  int randomNumber[numBands] ; // create and array that is the total number of bands long. if we want 4 bands, we have to have 4 random colors.
//  int randomNumber2[numBands] ; // create and array that is the total number of bands long. if we want 4 bands, we have to have 4 random colors.
//  for (int i = 0; i < numBands; i++) {
//    randomNumber[i] = random(255);
//    randomNumber2[i] = random(200, 255);
//  }
//  for (int i = 0; i <= NUM_LEDS / numBands; i++) {
//    leds[i] = CHSV(randomNumber[0], randomNumber2[0], 255);
//  }
//
//  if (numBands <= 6) {      //random colors for bands less then six
//    Serial.println("in bands less then 6");
//    for (int j = 1; j < numBands; j++) {
//      for (int i = ((NUM_LEDS / numBands) * j) + 1; i <= (NUM_LEDS / numBands) * (j + 1) ; i++) {
//        leds[i] = CHSV(randomNumber[j], randomNumber2[j], 255);
//      }
//    }
//  }
//  else if (numBands <= 30) {  //more then 6 bands, we repeat 3 colors
//    Serial.println("in bands less then 30");
//
//    for (int j = 1; j < numBands; j++) {
//      for (int i = ((NUM_LEDS / numBands) * j) + 1; i <= (NUM_LEDS / numBands) * (j + 1) ; i++) {
//        leds[i] = CHSV(randomNumber[j % 3], randomNumber2[j], 255);
//      }
//    }
//  }  else if (numBands > 30) {  // full saturation when they get close to each other
//    Serial.println("in bands greater then 30");
//
//    for (int j = 1; j <= numBands; j++) {
//      for (int i = ((NUM_LEDS / numBands) * j) + 1; i <= (NUM_LEDS / numBands) * (j + 1) ; i++) {
//        leds[i] = CHSV(randomNumber[j % 3], 255, 255);
//      }
//    }
//  }
//  FastLED.show();
//}
//
//// scrolling the colors down the line
//void barberPole() {
//  for (int i = 0; i < NUM_LEDS; i++)
//  {
//    leds[i] = leds[(i + 1) % NUM_LEDS];
//  }
//}
//
//void easterEggPole() { // a secret effect if you press FLASH 3 times
//  int numBands = 12;
//  CRGB barberPoleColors[4] = {CRGB::Red, CRGB::White, CRGB::Blue, CRGB::White} ; // create an  array for the 3 colors
//  //  for (int i = 0; i < numBands; i++) {
//  //    randomNumber[i] = random(255);
//  //    randomNumber2[i] = random(200, 255);
//  //  }
//  for (int i = 0; i <= NUM_LEDS / numBands; i++) {
//    leds[i] = barberPoleColors[0];
//  }
//  for (int j = 1; j < numBands; j++) {
//    for (int i = ((NUM_LEDS / numBands) * j) + 1; i <= (NUM_LEDS / numBands) * (j + 1) ; i++) {
//      leds[i] =  barberPoleColors[j % 4];
//    }
//  }
//}
