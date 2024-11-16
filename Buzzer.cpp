#include "Buzzer.h"
#include <Arduino.h>

Buzzer::Buzzer(int pin)
    : pin(pin), isActive(false), frequency(2000) {}

void Buzzer::init() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW); // Asegura que el buzzer esté apagado al inicio
}

void Buzzer::setFrequency(int freq) {
    frequency = freq; // Actualiza la frecuencia
}

void Buzzer::turnOn() {
    if (!isActive) {
        isActive = true;
        tone(pin, frequency); // Genera un tono en el buzzer
    }
}

void Buzzer::turnOff() {
    if (isActive) {
        isActive = false;
        noTone(pin); // Detiene el tono
    }
}
