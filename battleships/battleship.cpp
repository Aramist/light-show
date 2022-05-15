#include "battleship.h"

template <typename T>
Stack<T>::Stack(int maxSize)
{
    arr = new T *[maxSize];
    maxSize = maxSize;
}

template <typename T>
void Stack<T>::pop()
{
    if (size == 0)
        return;
    T *retVal = peek();
    start = (start + 1) % maxSize;
    size--;
    delete retVal;
}

template <typename T>
void Stack<T>::push(T *obj)
{
    int insertIdx = (start + size) % maxSize;
    arr[insertIdx] = obj;
    size++;
}

void Ship::draw(CRGB *ledStrip)
{
    CHSV color;
    if (team)
        color = rgb2hsv_approximate(CRGB::Red);
    else
        color = rgb2hsv_approximate(CRGB::Blue);
    CHSV dimColor = color;
    dimColor.v = 40;

    if (location < 20 || location > 250)
        return;

    int offset = team ? -1 : 1;
    // Treating team 0 as the team closer to led[0]
    // Basically mirrors the ship for the other team
    hsv2rgb_rainbow(health < 4 ? dimColor : color, ledStrip[location - 2 * offset]);
    hsv2rgb_rainbow(health < 2 ? dimColor : color, ledStrip[location - 1 * offset]);
    hsv2rgb_rainbow(health < 3 ? dimColor : color, ledStrip[location]);
    hsv2rgb_rainbow(health < 2 ? dimColor : color, ledStrip[location + 1 * offset]);
    ledStrip[location + 2 * offset] = CRGB::White;
}

void Ship::updateState()
{
    // Randomly move forward with prob 10%
    // On average, should travel 3 pixels per second (at 30fps)
    if (!random(0, 10))
    {
        if (team)
            location--;
        else
            location++;
    }
}

void Ship::explode() {} // TODO: this

Team::Team(uint8_t id)
{
    identity = id;
    fleet = new Stack<Ship>(4);
    bullets = new Stack<uint16_t>(10);
}

void Team::spawn()
{
    int loc = identity ? 240 : 50;
    Ship *newShip = new Ship(loc, identity);
    fleet->push(newShip);
}

void Team::draw(CRGB *ledStrip)
{
    for (int i = 0; i < fleet->size; i++)
    {
        int idx = (fleet->start + i) % fleet->maxSize;
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

    for (int i = 0; i < bullets->size; i++)
    {
        int idx = (bullets->start + i) % bullets->maxSize;
        *(bullets->arr[idx])++;
    }
}

void Team::shoot()
{
    if (fleet->isEmpty()) return;
    uint16_t *initialPos = new uint16_t(fleet->peek()->getLocation());
    bullets->push(initialPos);
}