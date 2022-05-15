#include <Arduino.h>
#include "FastLED.h"

#ifndef BATTLESHIP_H
#define BATTLESHIP_H

class Ship
{
private:
    uint16_t location;
    uint8_t health;
    uint8_t team;

public:
    Ship(int location, int team)
    {
        this->location = location;
        health = 4;
        this->team = team;
    };
    void draw(CRGB *ledStrip);
    void updateState();
    inline void hit()
    {
        if (health > 0)
            health--;
    };
    void explode();
    inline uint16_t getLocation() {return location;};
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
    inline T *peek() { return (arr[start]); };
    void pop();
    void push(T *);
    inline uint8_t isFull() { return size >= maxSize; };
    inline uint8_t isEmpty() { return size == 0; };
    // void iterate( std::function<void(T*)>);  // Calls the provided func on every element of the stack
};

class Team
{
private:
    Stack<Ship> *fleet;
    Stack<uint16_t> *bullets; // locations of bullets belonging to the team
    // CRGB color;
    uint8_t identity;

public:
    Team(uint8_t id);
    ~Team()
    {
        delete fleet;
        delete bullets;
    };
    void shoot();
    void spawn();
    void draw(CRGB *ledStrip);
    void updateState();
};

#endif