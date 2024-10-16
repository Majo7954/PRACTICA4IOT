#ifndef INVERNADERO_H
#define INVERNADERO_H

#include "ConexionWiFi.h"
#include "LED.h"
#include "Buzzer.h"
#include "Sensor.h"
#include <PubSubClient.h>

class Invernadero {
  private:
    PubSubClient cliente;
    Sensor sensor;
    bool fuegoDetectado;
    LED led;
    Buzzer buzzer;

  public:
    Invernadero(WiFiClient& clienteWiFi, int pinSensor);
    void configurarMQTT(const char* servidor, int puerto, const char* idCliente);
    void establecerCallback(void (*callback)(char*, byte*, unsigned int));
    void publicar(const char* topico, const char* mensaje);
    void ejecutar();
    void verificarSensorFuego();
    void controlarActuadores(bool estado);
    bool estaConectado();
    void reconectar(const char* topicoMQTT, const char* topicoControl);
};

#endif
