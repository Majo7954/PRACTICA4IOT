#include "Sensor.h"
#include <Arduino.h>

Sensor::Sensor(int pin) : pin(pin), estado(false) {
    pinMode(pin, INPUT);
}

bool Sensor::leer() {
    int valor = digitalRead(pin);
    return valor == 0;
}

bool Sensor::getEstado() {
    return estado;
}

void Sensor::setEstado(bool nuevoEstado) {
    estado = nuevoEstado;
}
