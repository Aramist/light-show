#include "battleship.h"
#include <FastLED.h>

#define DATA_PIN 3
#define NUM_LEDS 300

CRGB leds[NUM_LEDS];
ParticleManager exploder;
Team red(1, &exploder);
Team blue(0, &exploder);

void setup()
{
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.show();

    randomSeed(analogRead(0));

    Serial.begin(9600);
    /*
    while (!Serial);
    while (!Serial.available());
    Serial.read();
    Serial.read();
    */
}

uint64_t nextSpawn = 0;
uint64_t nextBullet = 0;

void loop()
{
    clearFrame();

    uint64_t curTime = millis();
    if (curTime > nextSpawn)
    {
        if (random(0, 2))
            red.spawn();
        else
            blue.spawn();
        nextSpawn = curTime + random(1500, 3000);
    }

    if (curTime > nextBullet)
    {
        if (random(0, 2))
            red.shoot();
        else
            blue.shoot();
        nextBullet = curTime + random(500, 1500);
    }

    // When the leading blue bullet hits the leading red ship, get rid of it and vice versa
    if (!blue.bullets->isEmpty() && red.hitTest(*(blue.bullets->peek())))
        blue.popBullet();
    if (!red.bullets->isEmpty() && blue.hitTest(*(red.bullets->peek())))
        red.popBullet();

    // If the front ships collide, kill both
    if (red.hitTest(blue.fleet->peek()->getLocation())) {
        red.fleet->peek()->kill();
        blue.fleet->peek()->kill();
    }


    red.updateState();
    blue.updateState();
    exploder.updateState();

    red.draw(leds);
    blue.draw(leds);
    exploder.draw(leds);

    /*
    if (Serial.available()) {
        red.shoot();
        Serial.read();
        Serial.read();
    }
    */

    FastLED.show();
    delay(30);
}

inline void clearFrame()
{
    for (int i = 0; i < NUM_LEDS; i++)
        leds[i] = CRGB::Black;
}