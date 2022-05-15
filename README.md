# Light Show

A set of Arduino scripts I made for my LED strip.

## Description

A collection of animation scripts for a WS2812B LED strip. Makes use of the [FastLED](https://github.com/FastLED/FastLED) library to address the pixels.

## Getting Started

### Dependencies

* Arduino IDE or CLI
* [FastLED library](https://github.com/FastLED/FastLED) (can be installed directly through IDE)

### Materials
* WS2812B LED strip (or similarly individually addressable strip, with small code changes)
    * I used this [16.4ft LED strip with 300 pixels](https://www.amazon.com/dp/B01CDTEJBG)
* Power supply for LED strip
    * I used this [5V 10A DC power supply](https://www.amazon.com/dp/B01D8FM71S)
* Arduino
    * I used an [Arduino Uno REV3](https://www.amazon.com/dp/B008GRTSV6)

### Wiring
* Connect the negative terminal of the power supply to grounds of both the Arduino and the LED strip
* Connect the positive terminal of the power supply to the VCC terminal of the LED strip
* Connect one of the Arduino's DO ports to the DATA_IN terminal of the LED strip

### Installing

* Clone repo:
    ```
    git clone git@github.com:Aramist/light-show.git
    git clone https://github.com/Aramist/light-show.git
    ```
* Open desired script

### Executing program

* If necessary, change the DATA_PIN macro to match the DO pin connected to the DATA_IN terminal of the LED strip.
* If necessary, change the NUM_LEDS macro to match the LED count of your LED strip

## Acknowledgments

Inspiration, code snippets, etc.
* [FastLED](https://github.com/FastLED/FastLED)
* [Simple README Template](https://gist.github.com/DomPizzie/7a5ff55ffa9081f2de27c315f5018afc)

