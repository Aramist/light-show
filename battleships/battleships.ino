#include "battleship.h"
#include <FastLED.h>

#define DATA_PIN 3
#define NUM_LEDS 300

CRGB leds[NUM_LEDS];
Team red(0);

void setup()
{
    // put your setup code here, to run once:
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.show();

    red.spawn();
    red.updateState();
    red.draw(leds);
    FastLED.show();
}

void loop()
{
    // put your main code here, to run repeatedly:
    clearFrame();

    if (!random(0, 20)) red.shoot();

    red.updateState();
    red.draw(leds);
    FastLED.show();
    delay(30);
}

inline void clearFrame()
{
    for (int i = 0; i < NUM_LEDS; i++)
        leds[i] = CRGB::Black;
}