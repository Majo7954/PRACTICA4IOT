#include "LED.h"
#include <Arduino.h>

LED::LED(int pin) : pin(pin), tiempoAnterior(0), estadoLED(false) {
    pinMode(pin, OUTPUT);
}

void LED::encender() {
    digitalWrite(pin, HIGH);
    estadoLED = true;
}

void LED::apagar() {
    digitalWrite(pin, LOW);
    estadoLED = false;
}

void LED::parpadear(int tiempo) {
    unsigned long tiempoActual = millis();

    if (tiempoActual - tiempoAnterior >= tiempo) {
        if (estadoLED) {
            apagar();
        } else {
            encender();
        }
        tiempoAnterior = tiempoActual;
    }
}
