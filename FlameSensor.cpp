#include "FlameSensor.h"
#include <Arduino.h>

FlameSensor::FlameSensor(int pin) 
    : pin(pin), lastReading(false), lastReadTime(0), readInterval(5000) {}

void FlameSensor::init() {
    pinMode(pin, INPUT);
}

bool FlameSensor::checkFireDetected() {
    // Solo realiza la lectura si ha pasado el intervalo de tiempo definido
    unsigned long currentMillis = millis();
    if (currentMillis - lastReadTime < readInterval) {
        return lastReading; // Devuelve el último estado leído
    }

    // Actualiza el estado del sensor
    lastReadTime = currentMillis;
    bool currentReading = !digitalRead(pin); // Asume que el pin bajo indica fuego
    lastReading = currentReading;

    return lastReading;
}

bool FlameSensor::hasStateChanged() {
    static bool previousState = false;
    bool currentState = checkFireDetected(); // Usa el método principal para obtener el estado
    if (currentState != previousState) {
        previousState = currentState;
        return true; // Detecta que hubo un cambio de estado
    }
    return false; // No hubo cambio
}