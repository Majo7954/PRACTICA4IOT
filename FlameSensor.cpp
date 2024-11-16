#include "FlameSensor.h"
#include <Arduino.h>

FlameSensor::FlameSensor(int pin) 
    : pin(pin), lastReading(false), lastReadTime(0), readInterval(5000) {}

void FlameSensor::init() {
    pinMode(pin, INPUT);
}

bool FlameSensor::checkFireDetected() {
    static unsigned long debounceTime = 1000;
    bool currentReading = !digitalRead(pin);

    if (currentReading && millis() - lastReadTime >= debounceTime) {
        lastReading = true;
        lastReadTime = millis();
    } else if (!currentReading) {
        lastReading = false;
    }

    return lastReading;
}


bool FlameSensor::hasStateChanged() {
    static bool previousState = false;
    bool currentState = checkFireDetected();
    if (currentState != previousState) {
        previousState = currentState;
        return true;
    }
    return false;
}
