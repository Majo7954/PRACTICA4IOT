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
    PubSubClient mqttClient;
    LED* led;
    Buzzer* buzzer;
    FlameSensor* flameSensor;
    bool alarmEnabled;
    bool alarmActive;
    StaticJsonDocument<200> outputDoc;
    char outputBuffer[256];
    
    const char* updateTopic;
    const char* deltaTopic;
    const char* acceptedTopic;
    const char* rejectedTopic;

    void handleDeltaMessage(char* payload);
    static void mqttCallback(char* topic, byte* payload, unsigned int length);
    void handleCallback(char* topic, byte* payload, unsigned int length);
    
public:
    Greenhouse(WiFiClientSecure* wifiClient, LED* led, Buzzer* buzzer, FlameSensor* flameSensor);
    void init();
    void configureMQTT(const char* broker, int port, const char* clientId, 
                      const char* updateTopic, const char* deltaTopic,
                      const char* acceptedTopic, const char* rejectedTopic);
    void checkFlameSensor();
    void updateActuators();
    void setAlarmEnabled(bool enabled);
    bool isConnected();
    void reconnect(const char* clientId);
    void reportState();
    void run();
};

#endif
