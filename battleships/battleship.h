#include <Arduino.h>
#include "FastLED.h"

#ifndef BATTLESHIP_H
#define BATTLESHIP_H
#define RIGHT_CUTOFF 230
#define LEFT_CUTOFF 40

#define MAX_CONCURRENT_EXPLOSIONS 2
#define PARTICLES_PER_EXPLOSION 8
#define TOTAL_PARTICLES (MAX_CONCURRENT_EXPLOSIONS * PARTICLES_PER_EXPLOSION)
#define PARTICLE_LIFESPAN 500
#define PARTICLE_DRAG 0.3
#define DT 0.033

class ParticleManager
{
private:
    uint8_t mask[MAX_CONCURRENT_EXPLOSIONS] = {0};
    float locations[TOTAL_PARTICLES] = {0};
    float velocities[TOTAL_PARTICLES] = {0};
    uint64_t birthtime[MAX_CONCURRENT_EXPLOSIONS] = {0};

public:
    void makeExplosion(uint16_t location, uint8_t teamIdentity);
    void updateState();
    void draw(CRGB *ledStrip);
    inline uint8_t isValid(uint8_t index) { return (millis() - birthtime[index / PARTICLES_PER_EXPLOSION]) < PARTICLE_LIFESPAN && mask[index / PARTICLES_PER_EXPLOSION]; };
    inline uint8_t brightnessForParticle(uint8_t index) { return cos8((uint8_t)((float)(millis() - birthtime[index]) / (float)PARTICLE_LIFESPAN) * 128); }
};

class Ship
{
private:
    uint16_t location;
    uint8_t health;
    uint8_t team;
    uint8_t movementCounter;

public:
    Ship(int location, int team)
    {
        this->location = location;
        health = 4;
        this->team = team;
    };
    void draw(CRGB *ledStrip);
    void updateState();
    inline uint8_t hitTest(uint16_t bullet)
    {
        if (bullet >= location - 2 && bullet <= location + 2)
            return 1;
        return 0;
    }
    inline void hit()
    {
        if (health > 0)
            health--;
    };
    inline void kill() { health = 0; };
    inline uint8_t shouldExplode() { return health == 0; };
    inline uint16_t getLocation() { return location; };
    // Prevent them from getting too close to enemy spawn point
    inline uint8_t isValid() { return location >= LEFT_CUTOFF + (team ? 50 : 0) && location <= RIGHT_CUTOFF + (team ? 0 : 50) && health > 0; };
};

template <typename T>
struct Stack
{
    Stack(int maxSize);
    ~Stack() { delete[] arr; };
    T **arr;
    int start = 0;
    int size = 0;
    int maxSize;
    inline T *peek()
    {
        if (isEmpty())
            return nullptr;
        else
            return arr[start];
    };
    void pop();
    void push(T *);
    inline uint8_t isFull() { return size >= maxSize; };
    inline uint8_t isEmpty() { return size == 0; };
    // void iterate( std::function<void(T*)>);  // Calls the provided func on every element of the stack
};

class Team
{
public:
    Stack<Ship> *fleet;
    Stack<uint16_t> *bullets; // locations of bullets belonging to the team
    ParticleManager *exploder;
    uint8_t identity;
    Team(uint8_t id, ParticleManager *exploder);
    ~Team()
    {
        delete fleet;
        delete bullets;
    };
    void shoot();
    void spawn();
    void draw(CRGB *ledStrip);
    void updateState();
    uint8_t hitTest(uint16_t location);
    void popBullet();
};

#endif