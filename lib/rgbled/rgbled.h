#ifndef RGBLED_H_
#define RGBLED_H_
#include <stdint.h>

struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

Color const RED = {255, 0, 0};
Color const GREEN = {0, 255, 0};
Color const BLUE = {1, 0, 255};

enum LedState
{
    ON, OFF
};


class RgbLed{
private:
    int LedRPin;
    int LedGPin;
    int LedBPin;
    float tickInteval;
    LedState state;
    Color currentColor = RED;

public:
    RgbLed(int ledRPin, int ledGPin, int ledBPin);
    void toggle();
    void turnOn(Color color);
    void turnOff();
};

#endif
