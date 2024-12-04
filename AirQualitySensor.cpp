#include "AirQualitySensor.h"
#include <Arduino.h>

AirQualitySensor::AirQualitySensor(int pin, int threshold)
    : pin(pin), threshold(threshold), lastReading(0) {}

void AirQualitySensor::init() {
    pinMode(pin, INPUT);
}

int AirQualitySensor::readPPM() {
    int analogValue = analogRead(pin);  // Lee el valor analógico del sensor
    lastReading = analogValue;         // Guarda la última lectura
    
    // Devuelve 1 si supera el umbral, 0 en caso contrario
    return (analogValue >= threshold) ? 1 : 0;
}

int AirQualitySensor::getLastReading() const {
    return lastReading; // Retorna la última lectura sin procesar
}