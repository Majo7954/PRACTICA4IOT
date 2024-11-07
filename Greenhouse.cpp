#include "Greenhouse.h"

Greenhouse::Greenhouse(WiFiClientSecure* wifiClient, LED* led, Buzzer* buzzer, FlameSensor* flameSensor)
    : mqttClient(*wifiClient), led(led), buzzer(buzzer), flameSensor(flameSensor),
      alarmEnabled(true), alarmActive(false) {
    
    mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->handleCallback(topic, payload, length);
    });
}

void Greenhouse::init() {
    led->init();
    buzzer->init();
    flameSensor->init();
}

void Greenhouse::configureMQTT(const char* broker, int port, const char* clientId, 
                                         const char* updateTopic, const char* deltaTopic,
                                         const char* acceptedTopic, const char* rejectedTopic) {
    this->updateTopic = updateTopic;
    this->deltaTopic = deltaTopic;
    this->acceptedTopic = acceptedTopic;
    this->rejectedTopic = rejectedTopic;
    
    mqttClient.setServer(broker, port);
    
    if (!mqttClient.connected()) {
        reconnect(clientId);
    }
}

void Greenhouse::checkFlameSensor() {
    if (flameSensor->hasStateChanged()) {
        bool fireDetected = flameSensor->checkFireDetected();
        alarmActive = fireDetected && alarmEnabled;
        reportState();
    }
}

void Greenhouse::updateActuators() {
    if (alarmActive && alarmEnabled) {
        led->blink();
        buzzer->turnOn();
    } else {
        led->turnOff();
        buzzer->turnOff();
    }
    reportState();
}

void Greenhouse::setAlarmEnabled(bool enabled) {
    alarmEnabled = enabled;

    if (enabled) {
        alarmActive = true;
    } else {
        alarmActive = false;
    }
    
    updateActuators();
    reportState();
}

bool Greenhouse::isConnected() {
    return mqttClient.connected();
}

void Greenhouse::reconnect(const char* clientId) {
    while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (mqttClient.connect(clientId)) {
            Serial.println("connected");
            mqttClient.subscribe(deltaTopic);
            mqttClient.subscribe(acceptedTopic);
            mqttClient.subscribe(rejectedTopic);
            reportState();
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
}

void Greenhouse::handleDeltaMessage(char* payload) {
    StaticJsonDocument<200> deltaDoc;
    DeserializationError error = deserializeJson(deltaDoc, payload);
    
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }
    
    // Procesa los cambios recibidos en el shadow
    JsonObject state = deltaDoc["state"];
    if (state.containsKey("alarmEnabled")) {
        bool alarmSetting = state["alarmEnabled"].as<bool>();
        setAlarmEnabled(alarmSetting);  // Establece `alarmEnabled`

        // Si `alarmEnabled` es `true`, activa `alarmActive` y los actuadores
        if (alarmSetting) {
            alarmActive = true;
            updateActuators();  // Activa el LED y el buzzer si `alarmEnabled` está activo
        }
    }
}

void Greenhouse::handleCallback(char* topic, byte* payload, unsigned int length) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    
    if (strcmp(topic, deltaTopic) == 0) {
        handleDeltaMessage(message);
    }
    else if (strcmp(topic, acceptedTopic) == 0) {
        Serial.println("Shadow update accepted");
    }
    else if (strcmp(topic, rejectedTopic) == 0) {
        Serial.println("Shadow update rejected");
    }
}

void Greenhouse::reportState() {
    static unsigned long lastReportTime = 0;
    unsigned long currentMillis = millis();

    // Enviar datos cada 10 segundos
    if (currentMillis - lastReportTime >= 10000) {
        outputDoc.clear();
        JsonObject state = outputDoc.createNestedObject("state");
        JsonObject reported = state.createNestedObject("reported");

        // Incluye solo las propiedades relevantes
        reported["alarmEnabled"] = alarmEnabled;
        reported["alarmActive"] = alarmActive;
        reported["fireDetected"] = flameSensor->checkFireDetected();
        reported["ledState"] = led->getState();
        reported["buzzerState"] = buzzer->getState();

        serializeJson(outputDoc, outputBuffer);
        mqttClient.publish(updateTopic, outputBuffer);
        
        // Actualiza el tiempo de último envío
        lastReportTime = currentMillis;
    }
}

void Greenhouse::run() {
    if (!mqttClient.connected()) {
        reconnect("ESP32_FireAlarm");
    }
    mqttClient.loop();
    
    checkFlameSensor();
    updateActuators();
    reportState();
}
