#include <Arduino.h>
#include "battleship.h"

void ParticleManager::draw(CRGB *ledStrip)
{
    for (int i = 0; i < TOTAL_PARTICLES; i++)
    {
        // Don't draw stale particles
        if (!isValid(i))
            continue;

        uint8_t brightness = this->brightnessForParticle(i);
        CHSV baseColor;
        if (i < PARTICLES_PER_EXPLOSION)
            baseColor = CHSV(HUE_BLUE, 100, brightness); // TODO: don't hardcode team colors in??
        else
            baseColor = CHSV(HUE_RED, 100, brightness);

        hsv2rgb_rainbow(baseColor, ledStrip[(int)locations[i]]);
    }
}

void ParticleManager::updateState()
{
    for (int i = 0; i < TOTAL_PARTICLES; i++)
    {
        // Don't draw stale particles
        if (!isValid(i)) // I forgot why mask exists concurrently with birthtime
            mask[i / PARTICLES_PER_EXPLOSION] = 0;

        locations[i] += DT * velocities[i];
        velocities[i] *= 1 - PARTICLE_DRAG;
    }
}

void ParticleManager::makeExplosion(uint16_t location, uint8_t teamIdentity)
{
    mask[teamIdentity] = 1;
    birthtime[teamIdentity] = millis();

    for (int i = PARTICLES_PER_EXPLOSION * teamIdentity; i < PARTICLES_PER_EXPLOSION * (teamIdentity + 1); i++)
    {
        locations[i] = (float)location;
        velocities[i] = random(10, 65);
        if (random(0, 2) == 1)
            velocities[i] *= -1;
    }
}

template <typename T>
Stack<T>::Stack(int maxSize)
{
    this->arr = new T *[maxSize];
    this->maxSize = maxSize;
    this->size = 0;
    this->start = 0;
}

template <typename T>
void Stack<T>::pop()
{
    if (this->size == 0)
        return;
    T *retVal = peek();
    this->start = (this->start + 1) % this->maxSize;
    this->size--;
    delete retVal;
}

template <typename T>
void Stack<T>::push(T *obj)
{
    if (isFull())
        return;
    int insertIdx = (this->start + this->size) % this->maxSize;
    this->arr[insertIdx] = obj;
    this->size++;
}

void Ship::draw(CRGB *ledStrip)
{
    CHSV color;
    if (team)
        color = rgb2hsv_approximate(CRGB::Red);
    else
        color = rgb2hsv_approximate(CRGB::Blue);
    color.v = 180;
    CHSV dimColor = color;
    dimColor.v = 20;

    if (location < LEFT_CUTOFF || location > RIGHT_CUTOFF)
        return;

    int offset = team ? -1 : 1;
    // Treating team 0 as the team closer to led[0]
    // Basically mirrors the ship for the other team
    hsv2rgb_rainbow(health < 4 ? dimColor : color, ledStrip[location - 2 * offset]);
    hsv2rgb_rainbow(health < 2 ? dimColor : color, ledStrip[location - 1 * offset]);
    hsv2rgb_rainbow(health < 3 ? dimColor : color, ledStrip[location]);
    hsv2rgb_rainbow(health < 2 ? dimColor : color, ledStrip[location + 1 * offset]);
    hsv2rgb_rainbow(CHSV(0, 0, health * 60), ledStrip[location + 2 * offset]);
}

void Ship::updateState()
{
    movementCounter = (movementCounter + 1) % 3;
    if (movementCounter == 0)
    {
        if (team)
            (this->location) -= 1;
        else
            (this->location) += 1;
    }
}

Team::Team(uint8_t id, ParticleManager *exploder)
{
    this->identity = id;
    this->exploder = exploder;
    this->fleet = new Stack<Ship>(4);
    this->bullets = new Stack<uint16_t>(10);
}

void Team::spawn()
{
    if (fleet->isFull())
        return;
    int loc = this->identity ? RIGHT_CUTOFF : LEFT_CUTOFF;
    Ship *newShip = new Ship(loc, identity);
    fleet->push(newShip);
}

void Team::draw(CRGB *ledStrip)
{
    CRGB teamColor = identity ? CRGB::Red : CRGB::Blue;
    for (int i = 0; i < bullets->size; i++)
    {
        int idx = (bullets->start + i) % (bullets->maxSize);
        uint16_t loc = *(bullets->arr[idx]);
        ledStrip[loc] = teamColor;
    }

    for (int i = 0; i < fleet->size; i++)
    {
        int idx = (int)((fleet->start + i) % fleet->maxSize);
        fleet->arr[idx]->draw(ledStrip);
    }
}

void Team::updateState()
{
    for (int i = 0; i < fleet->size; i++)
    {
        int idx = (fleet->start + i) % fleet->maxSize;
        fleet->arr[idx]->updateState();
    }
    if (!(fleet->peek()->isValid()))
    {
        if (fleet->peek()->shouldExplode())
            exploder->makeExplosion(fleet->peek()->getLocation(), identity);
        fleet->pop();
    }

    for (int i = 0; i < bullets->size; i++)
    {
        int idx = (bullets->start + i) % bullets->maxSize;
        if (identity)
            *(bullets->arr[idx]) -= 1;
        else
            *(bullets->arr[idx]) += 1;

        if (*(bullets->peek()) > RIGHT_CUTOFF + 1 || *(bullets->peek()) < LEFT_CUTOFF - 1)
            bullets->pop();
    }
}

uint8_t Team::hitTest(uint16_t location)
{
    if (fleet->isEmpty())
        return 0;
    if (fleet->peek()->hitTest(location))
    {
        // The bullet hit us
        fleet->peek()->hit();
        return 1; // Let the caller know the bullet should be culled
    }

    return 0;
}

void Team::popBullet()
{
    bullets->pop();
}

void Team::shoot()
{
    if (fleet->isEmpty() || bullets->isFull())
        return;
    uint16_t *initialPos = new uint16_t(fleet->peek()->getLocation());
    *initialPos += identity ? -3 : 3;

    bullets->push(initialPos);
}