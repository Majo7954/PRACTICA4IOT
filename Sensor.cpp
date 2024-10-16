#include "Sensor.h"
#include <Arduino.h>

Sensor::Sensor(int pin) : pin(pin), estado(false) {
    pinMode(pin, INPUT);
}

bool Sensor::leerSensor() {
    int valor = digitalRead(pin);
    return valor == 0;
}

bool Sensor::obtenerEstado() {
    return estado;
}

void Sensor::establecerEstado(bool nuevoEstado) {
    estado = nuevoEstado;
}
