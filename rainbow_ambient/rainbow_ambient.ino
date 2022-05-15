#include "FastLED.h"
#define NUM_LEDS 300
#define DATA_PIN 3

// Animation constants
#define NUM_CYCLES 30
#define BREATH_PERIOD 10.0

CRGB leds[NUM_LEDS];
uint8_t phases[NUM_LEDS];
uint8_t bias = 20;

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.show();

  for(int i = 0; i < NUM_LEDS; i++)
    phases[i] = phaseForIndex(i);

  // delay(2000);
}

void loop() {
  // ms * (1 sec / 1000ms) * (1 period / BREATH_PERIOD sec) * (256 ticks / 1 period)
  int ticks = (int) (millis() / 1000.0 / BREATH_PERIOD * 256);
  uint8_t t = (uint8_t) (ticks % 256);
  for (int i=0; i<NUM_LEDS; i++){
    uint8_t intensity = quadwave8(t + phases[i]);
    if (intensity < bias)
      intensity = 0;
    else
      intensity -= bias;

    leds[i] = CHSV( hueForIndex(i), 255, intensity);
  }
  FastLED.show();
  delay(20);
}

uint8_t phaseForIndex(int index) {
  float floatPhase = ((float) index) / ((float) NUM_LEDS) * 256 * NUM_CYCLES;
  return (uint8_t) ((int) floatPhase % 256);
}

uint8_t hueForIndex(int index) {
  return (uint8_t) (((float) index) / ((float) NUM_LEDS) * 256);
}
