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

    mqttClient.subscribe("ESP32/commands");
}


void Greenhouse::checkFlameSensor() {
    if (flameSensor->hasStateChanged()) {
        bool fireDetected = flameSensor->checkFireDetected();
        Serial.print("Fuego detectado: ");
        Serial.println(fireDetected);

        if (fireDetected) {
            alarmEnabled = true;
            alarmActive = true;
            Serial.println("Alarma activada.");
        } else if (!fireDetected && alarmEnabled) {
            alarmActive = false;
            Serial.println("Alarma desactivada.");
        }

        updateActuators();
        reportState();
    }
}



void Greenhouse::updateActuators() {
    if (alarmActive && alarmEnabled) {
        led->blink();
        buzzer->setFrequency(2000);
        buzzer->turnOn();
    } else {
        led->turnOff();
        buzzer->turnOff();
    }
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
    
    JsonObject state = deltaDoc["state"];
    if (state.containsKey("alarmEnabled")) {
        bool alarmSetting = state["alarmEnabled"].as<bool>();
        setAlarmEnabled(alarmSetting);

        if (alarmSetting) {
            alarmActive = true;
            updateActuators();
        }
    }
}

void Greenhouse::handleCallback(char* topic, byte* payload, unsigned int length) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    
    if (strcmp(topic, deltaTopic) == 0) {
        handleDeltaMessage(message);
    } else if (strcmp(topic, "ESP32/commands") == 0) {
        StaticJsonDocument<200> commandDoc;
        DeserializationError error = deserializeJson(commandDoc, message);

        if (!error) {
            const char* command = commandDoc["command"];
            if (strcmp(command, "activateAlarm") == 0) {
                alarmEnabled = true;
                alarmActive = true;
                updateActuators();
            } else if (strcmp(command, "deactivateAlarm") == 0) {
                alarmActive = false;
                updateActuators();
            }
        }
    }
}


void Greenhouse::reportState() {
    static unsigned long lastReportTime = 0;
    unsigned long currentMillis = millis();

    if (currentMillis - lastReportTime >= 10000) {
        outputDoc.clear();
        JsonObject state = outputDoc.createNestedObject("state");
        JsonObject reported = state.createNestedObject("reported");

        reported["alarmEnabled"] = alarmEnabled;
        reported["alarmActive"] = alarmActive;
        reported["fireDetected"] = flameSensor->checkFireDetected();
        reported["ledState"] = led->getState();
        reported["buzzerState"] = buzzer->getState();

        serializeJson(outputDoc, outputBuffer);
        mqttClient.publish(updateTopic, outputBuffer);
        
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
