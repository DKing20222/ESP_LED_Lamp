#include <FastLED.h>

#define LED_PIN 27
#define LED_NUM 58
#define BTN_PIN 23
#define POT_COLOR 25

#define EFFECTS_NUM 6

volatile int mode = -1;
int brightness = 255;
int speed = 10;

int beatsinCorrection = -65536 / 4;

CRGB leds[LED_NUM];

unsigned long lastPressTime = 0;
#define debounceTime 500

void IRAM_ATTR ModeSwitch() {
  unsigned long now = millis();
  if (now - lastPressTime > debounceTime) {
    mode++;
    if (mode > EFFECTS_NUM) {
      mode = -1;
    }
    lastPressTime = now;
  }
}

int readColor()
{
  int read = analogRead(POT_COLOR);
  return map(read, 0, 4095, 0, 255);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_NUM);
  FastLED.setBrightness(brightness);
  FastLED.setTemperature(ClearBlueSky);
  FastLED.setCorrection(TypicalPixelString);
  ResetLED();

  pinMode(BTN_PIN, INPUT);
  attachInterrupt(BTN_PIN, ModeSwitch, RISING);

  pinMode(POT_COLOR, INPUT);
}

void loop() {
  switch(mode)
  {
    case -1:
    // Turn off
      ResetLED();
      break;
    case 0:
      Lamp();
      break;
    case 1:
      Rainbow();
      break;
    case 2:
      ColorLamp();
      break;
    case 3:
      Line();
      break;
    case 4:
      LineRainbow();
      break;
    case 5:
      Pulse();
      break;
    case 6:
      Breath();
      break;
  }
  FastLED.show();
}

void ResetLED() {
  fill_solid(leds, LED_NUM, CRGB(0, 0, 0));
}

// Basic white light
void Lamp()
{
  fill_solid(leds, LED_NUM, CHSV(100, 0, 225));
}

// 
void Rainbow()
{
  fill_rainbow(leds, LED_NUM, millis() / (2 * speed), 1);
}

// User defined color by potentiometer
void ColorLamp()
{
  // fill_solid(leds, LED_NUM, CHSV(readColor(), 255, brightness));
  fill_solid(leds, LED_NUM, CHSV(0, 255, brightness));
}

// Single color line traveling from side to side
void Line()
{
  int pos = beatsin16(speed, 0, LED_NUM - 1, 0, beatsinCorrection);
  fadeToBlackBy(leds, LED_NUM, speed);
  // leds[pos] = CHSV(readColor(), 255, brightness);
  leds[pos] = CHSV(0, 255, brightness);
}

// Rainbow line traveling from side to side
void LineRainbow()
{
  int pos = beatsin16(speed, 0, LED_NUM - 1, 0, beatsinCorrection);
  fadeToBlackBy(leds, LED_NUM, speed);
  leds[pos] = CHSV(millis() / (2 * speed), 255, brightness);
}

// Two lines traveling from the center to the sides
void Pulse(){
  int j = (LED_NUM / 2)-1;
  for (uint8_t i = LED_NUM / 2; i < LED_NUM; i++) {
    leds[i] = CHSV(millis() / 10, 255, brightness);
    leds[j] = CHSV(millis() / 10, 255, brightness);
    j--;
    fadeToBlackBy(leds, LED_NUM, speed);
    FastLED.show();
    delay(20);
  }
}

// Brightness decreasing and increasing
void Breath()
{
  float breath = beatsin16(speed, 64, 255, 0, beatsinCorrection);
  // fill_solid(leds, LED_NUM, CHSV(readColor(), 255, breath));
  fill_solid(leds, LED_NUM, CHSV(0, 255, breath));
}
