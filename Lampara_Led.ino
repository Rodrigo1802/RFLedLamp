#include <SPI.h>
#include "RF24.h"
#include "FastLED.h"
#include <printf.h>

// Serial
char MAGIC_WORD[] = {'P', 'o', 'o', 'k', 'y'};
uint8_t XOR_CHK = 0x55; // 85

// Define the array of leds 
#define LED_PIN     2
#define NUM_LEDS    7
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG
CRGB leds[NUM_LEDS];

// RF
#define CE_PIN 7
#define CSN_PIN 8
byte addresses[][6] = {"1Node"};
RF24 radio(CE_PIN, CSN_PIN);

byte speed = 10;
long randNumber = -1;
byte rainbowCount = 0;
uint8_t countIdxGlobal = 0;

void setup() {
  initSerial();
  initLeds();
  initRF();
}

void initSerial() {
  Serial.begin(115200);
}

void initLeds() {
  Serial.println(F("Fast Led start"));
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);//.setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
}

void initRF() {
  printf_begin();
  Serial.println(F("RF24 start"));
  radio.begin();
  radio.setChannel(108);  // Above most Wifi Channels
  radio.setPALevel(RF24_PA_MIN);
  radio.printDetails();
}

void loop() {
  if(dataAvailable()) {
    readMagicWord();
    myLittlePonny();
  } else {
    randNumber = -1;
    randomColors();
  }
}

bool readMagicWord() {
  for(uint8_t i = 0; i < sizeof(MAGIC_WORD); ++i) {
    char c = syncRead();
    if(MAGIC_WORD[i] != c) {
      return false;
    }
  }
  byte hi = syncRead(); // '2' = 50
  byte lo = syncRead(); // '4' = 52
  byte chkSum = syncRead(); // 'S' = 83
  // if checksum does not match go back to wait
  return (chkSum != (hi ^ lo ^ XOR_CHK));
}

byte syncRead(){
  while(!dataAvailable());
  return dataRead();
}

bool dataAvailable() {
  return radio.available();
  //return Serial.available();
}

byte dataRead() {
  //return Serial.read();
  byte dataReceived;
  radio.read(&dataReceived, sizeof(dataReceived));
  return dataReceived;
}

void myLittlePonny() {
  blackSound(8500);
  
  for(int i=0; i<12; i++){
    whiteSound(100);
    blackSound(1100);
    whiteSound(100);
    blackSound(250);
    whiteSound(80);
    blackSound(860 - (i*10));
  }
  
  rainbowSound(60, 500);
  blackSound(1000);
}

void rainbowSound(uint8_t times, int delayMs){
  for (uint8_t i = 0; i < times; i++) {
    rainbow(i, delayMs);
  }
}
void whiteSound(int delayMs){
  rarity(0, delayMs);
}

void blackSound(int delayMs){
  black(0, delayMs);
}

void randomColors() {
  if(randNumber == 0 && rainbowCount > 0){
    rainbowCount--;
  } else { 
    randNumber = random(0, 7);
  }
  speed = min(max(speed, 0), 100);
  int delayMs = speed * 100;
  switch(randNumber){
    case 0:
      rainbowCount = 100;
      rainbow(countIdxGlobal, delayMs);
      break;
    case 1:
      rainbowDash(countIdxGlobal, delayMs);
      break;
    case 2:
      flutterShy(countIdxGlobal, delayMs);
      break;
    case 3:
      rarity(countIdxGlobal, delayMs);
      break;
    case 4:
      twilight(countIdxGlobal,  delayMs);
      break;
    case 5:
      pinkyPie(countIdxGlobal, delayMs);
      break;
    case 6:
      appleJack(countIdxGlobal, delayMs);
      break;
  }
  countIdxGlobal++;
}

void rainbow(uint8_t countIdx, int delayMs){
  leds[countIdx % NUM_LEDS] = CRGB( 30, 144, 255);
  leds[(countIdx+1) % NUM_LEDS] = CRGB( 255, 255, 0);
  leds[(countIdx+2) % NUM_LEDS] = CRGB( 128, 128, 128);
  leds[(countIdx+3) % NUM_LEDS] = CRGB( 139, 0, 139);
  leds[(countIdx+4) % NUM_LEDS] = CRGB( 255, 0, 255);
  leds[(countIdx+5) % NUM_LEDS] = CRGB( 255, 140, 0);
  FastLED.show();
  delay(delayMs);
}

void rainbowDash(uint8_t countIdx, int delayMs){
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(30, 144, 255);
  }
  FastLED.show();
  delay(delayMs);
}

void flutterShy(uint8_t countIdx, int delayMs){
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(255, 255, 0);
  }
  FastLED.show();
  delay(delayMs);
}

void rarity(uint8_t countIdx, int delayMs){
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(128, 128, 128);
  }
  FastLED.show();
  delay(delayMs);
}

void twilight(uint8_t countIdx, int delayMs){
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(139, 0, 139);
  }
  FastLED.show();
  delay(delayMs);
}

void pinkyPie(uint8_t countIdx, int delayMs){
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(255, 0, 255);
  }
  FastLED.show();
  delay(delayMs);
}

void appleJack(uint8_t countIdx, int delayMs){
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(255, 140, 0);
  }
  FastLED.show();
  delay(delayMs);
}

void black(uint8_t countIdx, int delayMs){
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
  delay(delayMs);
}

