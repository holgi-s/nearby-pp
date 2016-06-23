//
// Created by Holger on 07.05.2016.
//


#ifndef NEARBYCONNECTION_LEDCTRL_H
#define NEARBYCONNECTION_LEDCTRL_H

#include <stdint.h>

class CLedController {
public:
    CLedController(int redPin, int greenPin, int bluePin);
    ~CLedController();

    void AdjustRGB(float redFactor, float greenFactor, float blueFactor);

    void SetRGB(int red, int green, int blue);

private:
    void setup(int redPin, int greenPin, int bluePin);
    int color(int color, float factor);

    uint8_t _pinRed = 11;
    uint8_t _pinGreen = 13;
    uint8_t _pinBlue = 15;

    float _redFactor = 1.0f;
    float _greenFactor = 0.55f;
    float _blueFactor = 0.95f;

    int _fd = -1;
};


#endif //NEARBYCONNECTION_LEDCTRL_H
