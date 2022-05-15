#include "FastLED.h"
#include "stdlib.h"

#define NUM_LEDS 300
#define DATA_PIN 3

// Animation constants
#define GRAVITY 14
#define MAX_PROJS 8
#define MAX_PARTS 8
#define NUM_PARTS (MAX_PARTS * MAX_PROJS)
#define PART_LIFESPAN 1000
#define DRAG 0.1
#define PROJ_RADIUS 2
// about 30fps
#define REFRESH 33
#define DT 0.033
#define BRIGHTNESS 40

CRGB leds[NUM_LEDS];

CHSV projColors[MAX_PROJS];
uint8_t projMask[MAX_PROJS] = {0};
float projLocations[MAX_PROJS] = {0};
float projVelocities[MAX_PROJS] = {0};


// Explosion variables
uint8_t explosionMask[MAX_PROJS] = {0};
float particleLocations[NUM_PARTS] = {0};
float particleVelocities[NUM_PARTS] = {0};
uint64_t particleSpawnTime[MAX_PROJS] = {0};



void setup() {
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }

  for(int i = 0; i < MAX_PROJS; i++) {
    projColors[i] = CHSV(0, 0, 0);
  }
  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.show();

  randomSeed(analogRead(0));

  // Serial.begin(9600);
}



uint64_t nextSpawn = 0;

// char printBuf[50];

void loop() {
  if (millis() > nextSpawn) {
    spawnFirework();
    nextSpawn = millis() + random(500, 2000);
  }
  
  clear_leds();
  drawProjs();
  updateProjStates();

  drawParticles();
  updateParticleStates();

  FastLED.show();
  delay(REFRESH);
}


void clear_leds() {
  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB::Black;
  }
}


void drawProjs() {
  for (int i = 0; i < MAX_PROJS; i++) {
    if (!projMask[i] || explosionMask[i]) continue;
    int16_t proj_loc = (int16_t) projLocations[i];
    if (proj_loc < -PROJ_RADIUS || proj_loc > NUM_LEDS + PROJ_RADIUS) continue;
    
    int start_loc = max(0, proj_loc - PROJ_RADIUS);
    int end_loc = min(NUM_LEDS, proj_loc + PROJ_RADIUS);

    if (start_loc > NUM_LEDS || end_loc < 0) continue;

    for (int j = start_loc; j < end_loc; j++)
      hsv2rgb_rainbow(projColors[i], leds[j]);
  }
}

void updateProjStates() {
  for (int i = 0; i < MAX_PROJS; i++) {
    if (!projMask[i] || explosionMask[i]) continue;

    projVelocities[i] -= DT * GRAVITY;
    projLocations[i] += DT * projVelocities[i];
  }

  verifyProjStates();
}


void verifyProjStates() {
  for (int i = 0; i < MAX_PROJS; i++) {
    if (!projMask[i]) continue;

    if (projLocations[i] <= 0 || projLocations[i] >= NUM_LEDS) {
      projMask[i] = 0;
      continue;
    }

    if (projVelocities[i] < 5 && !explosionMask[i]) {
      makeExplosion(i);
    }
    
  }
}

void spawnFirework() {
  for (int i = 0; i < MAX_PROJS; i++) {
    // Find an index that's not taken
    if (projMask[i] || explosionMask[i]) continue;
    
    projMask[i] = 1;
    explosionMask[i] = 0;
    projVelocities[i] = 30 + random(0, 60);
    projLocations[i] = 1;
    projColors[i] = CHSV((uint8_t)random(0, 255), 255, BRIGHTNESS);
    break;
  }
}



void makeExplosion(int projIdx) {
  particleSpawnTime[projIdx] = millis();
  explosionMask[projIdx] = 1;
  // Indexes all particles belonging to said projectile
  for (int i=MAX_PARTS*projIdx; i<MAX_PARTS*(projIdx+1); i++){
    particleLocations[i] = projLocations[projIdx];
    particleVelocities[i] = random(10, 65);
    if (random(0, 2) == 1) particleVelocities[i] *= -1;
  }
}


void drawParticles() {
  uint64_t curTime = millis();
  for(int i = 0; i < NUM_PARTS; i++){
    if (!explosionMask[i/MAX_PARTS]) {
      i += MAX_PARTS - 1;
      continue;
    }
    int age = curTime - particleSpawnTime[i/MAX_PARTS];
    if (age > PART_LIFESPAN) {
      i += MAX_PARTS - 1;
      continue;
    }

    int loc = (int)particleLocations[i];
    if (loc < 0 || loc >= NUM_LEDS) continue;

    projColors[i / MAX_PARTS].v = brightnessForAge(age);
    hsv2rgb_rainbow(projColors[i / MAX_PARTS], leds[loc]);
  }
}

void updateParticleStates() {
  uint64_t curTime = millis();
  for(int i = 0; i < NUM_PARTS; i++){
    if (!explosionMask[i/MAX_PARTS]) {
      i += MAX_PARTS - 1;
      continue;
    }
    
    if (curTime - particleSpawnTime[i/MAX_PARTS] > PART_LIFESPAN) {
      projMask[i/MAX_PARTS] = 0;  // 0 means dead
      explosionMask[i/MAX_PARTS] = 0;
      i += MAX_PARTS - 1;
      continue;
    }

    particleVelocities[i] *= 1-DRAG;
    particleLocations[i] += DT * particleVelocities[i];
  }
}


uint8_t brightnessForAge(int age) {
  float ratio = ((float) age) / ((float) PART_LIFESPAN);
  uint8_t t = (uint8_t) (ratio * 128);
  // uint8_t t = (uint8_t) ((age * 256) / PART_LIFESPAN);
  return cos8(t);
}
