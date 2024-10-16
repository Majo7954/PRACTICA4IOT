#ifndef INVERNADERO_H
#define INVERNADERO_H

#include "WiFiConnection.h"
#include "LED.h"
#include "Buzzer.h"
#include "Sensor.h"
#include <PubSubClient.h>

class Invernadero {
  private:
    PubSubClient client;
    Sensor sensor;
    bool fuegoDetectado;
    LED led;
    Buzzer buzzer;

  public:
    Invernadero(WiFiClient& espClient, int sensorPin);
    void setupMQTT(const char* server, int port, const char* clientID);
    void setCallback(void (*callback)(char*, byte*, unsigned int));
    void publish(const char* topic, const char* message);
    void loop();
    void checkFireSensor();
    void controlActuadores(bool estado);
    bool isConnected();
    void reconnect(const char* mqttTopic, const char* controlTopic);
};

#endif
