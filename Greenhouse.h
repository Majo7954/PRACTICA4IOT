#ifndef GREENHOUSE_H
#define GREENHOUSE_H

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "LED.h"
#include "Buzzer.h"
#include "FlameSensor.h"
#include "WiFiConnection.h"

class Greenhouse {
private:
    PubSubClient mqttClient;          // Cliente MQTT
    LED* led;                         // Controlador LED
    Buzzer* buzzer;                   // Controlador del buzzer
    FlameSensor* flameSensor;         // Sensor de llama
    bool alarmEnabled;                // Estado de activación de la alarma
    bool alarmActive;                 // Estado de actividad de la alarma
    StaticJsonDocument<200> outputDoc; // Documento JSON para actualizar el shadow
    char outputBuffer[256];           // Buffer para serialización JSON

    // Tópicos MQTT
    const char* updateTopic;          
    const char* deltaTopic;
    const char* acceptedTopic;
    const char* rejectedTopic;

    // Métodos privados
    void handleDeltaMessage(char* payload);         // Maneja mensajes de delta del shadow
    void handleCallback(char* topic, byte* payload, unsigned int length); // Callback MQTT

public:
    // Constructor
    Greenhouse(WiFiClientSecure* wifiClient, LED* led, Buzzer* buzzer, FlameSensor* flameSensor);

    // Métodos públicos
    void init();                                    // Inicializa el sistema
    void configureMQTT(const char* broker, int port, const char* clientId, 
                       const char* updateTopic, const char* deltaTopic,
                       const char* acceptedTopic, const char* rejectedTopic); // Configura MQTT
    void checkFlameSensor();                       // Verifica el estado del sensor de llama
    void updateActuators();                        // Actualiza los actuadores según el estado
    void setAlarmEnabled(bool enabled);            // Cambia el estado de la alarma
    void publishSensorData(const char* sensorId, float value, const char* unit);
    bool isConnected();                            // Verifica conexión MQTT
    void reconnect(const char* clientId);          // Reintenta conexión MQTT
    void reportState();                            // Reporta el estado al shadow
    void run();                                    // Ejecuta el loop principal
};

#endif
