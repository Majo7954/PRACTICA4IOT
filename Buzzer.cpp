#include "Buzzer.h"
#include <Arduino.h>

Buzzer::Buzzer(int pin) 
    : pin(pin), isActive(false), frequency(1000) {}

void Buzzer::init() {
    pinMode(pin, OUTPUT);
    noTone(pin);
}

void Buzzer::turnOn() {
    tone(pin, frequency);
    isActive = true;
}

void Buzzer::turnOff() {
    noTone(pin);
    isActive = false;
}

void Buzzer::toggle() {
    if (isActive) {
        turnOff();
    } else {
        turnOn();
    }
}