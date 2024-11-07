#include "FlameSensor.h"
#include <Arduino.h>

FlameSensor::FlameSensor(int pin) 
    : pin(pin), lastReading(false), lastReadTime(0), readInterval(5000) {}

void FlameSensor::init() {
    pinMode(pin, INPUT);
}

bool FlameSensor::checkFireDetected() {
    if (millis() - lastReadTime >= readInterval) {
        // For a digital flame sensor:
        // LOW (0) generally indicates fire detected
        // HIGH (1) means no fire detected
        lastReading = !digitalRead(pin);  // Invert the reading: true means fire detected
        lastReadTime = millis();
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
