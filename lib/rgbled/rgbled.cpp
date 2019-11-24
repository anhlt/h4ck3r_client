#include "rgbled.h"
#include <Arduino.h>

RgbLed::RgbLed(int ledRPin, int ledGPin, int ledBPin)
{
    LedRPin = ledRPin;
    LedGPin = ledGPin;
    LedBPin = ledBPin;

    ledcSetup(0, 12800, 8);
    ledcAttachPin(ledRPin, 0);

    ledcSetup(1, 12800, 8);
    ledcAttachPin(ledGPin, 1);

    ledcSetup(2, 12800, 8);
    ledcAttachPin(ledBPin, 2);
};

void RgbLed::turnOn(Color color)
{


  // ledBをチャネル2へ接続
  state = ON;
  ledcWrite(0, color.r);
  ledcWrite(1, color.g);
  ledcWrite(2, color.b);
};

void RgbLed::turnOff()
{
  state = OFF;
  ledcWrite(0, 0);
  ledcWrite(1, 1);
  ledcWrite(2, 2);

};