//
// Created by Holger on 07.05.2016.
//

#include "ledctrl.h"
#include <iostream>

#include <pigpiod_if2.h>

CLedController::CLedController(int redPin, int greenPin, int bluePin) {
    int version = pigpiod_if_version();

    std::cout << "PiGPIOD Version: " << version << std::endl;
    std::cout << "PiGPIOD connecting to daemon..." << std::endl;

    _fd = pigpio_start(nullptr, nullptr);
    if(_fd >= 0){
        setup(redPin, greenPin, bluePin);
    } else {
        std::cerr << "PiGPIOD connection failed!" << std::endl;
    }
}

CLedController::~CLedController() {
    pigpio_stop(_fd);
    _fd = -1;
}

void CLedController::setup(int redPin, int greenPin, int bluePin) {

    _pinRed = redPin;
    _pinGreen = greenPin;
    _pinBlue = bluePin;

    int err = set_mode(_fd, _pinRed, PI_OUTPUT);
    if(err) {
        std::cerr << "set_mode pin: "<< _pinRed << " err: " << err << std::endl;
    }
    err = set_mode(_fd, _pinGreen, PI_OUTPUT);
    if(err) {
        std::cerr << "set_mode pin: "<< _pinGreen << " err: " << err << std::endl;
    }
    err = set_mode(_fd, _pinBlue, PI_OUTPUT);
    if(err) {
        std::cerr << "set_mode pin: "<< _pinBlue << " err: " << err << std::endl;
    }

    err = set_PWM_dutycycle(_fd, _pinRed, 0);
    if(err) {
        std::cerr << "set_PWM_dutycycle pin: "<< _pinRed << " err: " << err << std::endl;
    }
    err = set_PWM_dutycycle(_fd, _pinGreen, 0);
    if(err) {
        std::cerr << "set_PWM_dutycycle pin: "<< _pinGreen << " err: " << err << std::endl;
    }
    err = set_PWM_dutycycle(_fd, _pinBlue, 0);
    if(err) {
        std::cerr << "set_PWM_dutycycle pin: "<< _pinBlue << " err: " << err << std::endl;
    }

    std::cout << "PiGPIO set red pwm pin: " << redPin << std::endl;
    std::cout << "PiGPIO set green pwm pin: " << greenPin << std::endl;
    std::cout << "PiGPIO set blue pwm pin: " << bluePin << std::endl;

    std::cout << "PiGPIO setup complete" << std::endl;
}

int CLedController::color(int color, float factor) {
    return std::min<int>((int)((float)color*factor), 255);
}


void CLedController::SetRGB(int red, int green, int blue) {

    if(_fd != -1) {

        int err = set_PWM_dutycycle(_fd, _pinRed, color(red, _redFactor));
        if(err) {
            std::cerr << "set_PWM_dutycycle err: " << err << std::endl;
        }
        err = set_PWM_dutycycle(_fd, _pinGreen, color(green, _greenFactor));
        if(err) {
            std::cerr << "set_PWM_dutycycle err: " << err << std::endl;
        }
        err = set_PWM_dutycycle(_fd, _pinBlue, color(blue, _blueFactor));
        if(err) {
            std::cerr << "set_PWM_dutycycle err: " << err << std::endl;
        }
    }
}