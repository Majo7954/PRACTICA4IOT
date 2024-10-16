#include "Buzzer.h"
#include <Arduino.h>

Buzzer::Buzzer(int pin) : pin(pin) {
    pinMode(pin, OUTPUT);
}

void Buzzer::activar() {
    tone(pin, 1000);
}

void Buzzer::desactivar() {
    noTone(pin);
}
