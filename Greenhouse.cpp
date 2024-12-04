#include "Greenhouse.h"

unsigned long previousMillis = 0;  // Almacena el tiempo de la última publicación
const long interval = 5000;  // Intervalo de 5 segundos

Greenhouse::Greenhouse(WiFiClientSecure* wifiClient, LED* led, Buzzer* buzzer, 
                       FlameSensor* flameSensor, AirQualitySensor* airQualitySensor)
    : mqttClient(*wifiClient), led(led), buzzer(buzzer), flameSensor(flameSensor), 
      airQualitySensor(airQualitySensor), alarmEnabled(false), alarmActive(false),
      previousMillis(0), interval(5000) {
        mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->handleCallback(topic, payload, length);
    });
}


void Greenhouse::init() {
    led->init();
    buzzer->init();
    flameSensor->init();
    airQualitySensor->init(); // Inicializa el sensor de calidad de aire
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
        Serial.print("Fuego detectado: ");
        Serial.println(fireDetected);

        // Solo reportar el estado al shadow si algo ha cambiado
        reportState();
    }
}

void Greenhouse::updateActuators() {
    if (alarmActive) {
        led->blink();              // Enciende el LED en modo parpadeo
        buzzer->setFrequency(2000); // Establece la frecuencia del buzzer
        buzzer->turnOn();           // Activa el buzzer
    } else {
        led->turnOff();            // Apaga el LED
        buzzer->turnOff();         // Apaga el buzzer
    }
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
        Serial.print("Error al procesar mensaje delta: ");
        Serial.println(error.c_str());
        return;
    }
    
    JsonObject state = deltaDoc["state"];
    
    // Manejar la actualización del campo alarmEnabled
    if (state.containsKey("alarmEnabled")) {
        bool alarmSetting = state["alarmEnabled"].as<bool>();
        setAlarmEnabled(alarmSetting);
        
        // Confirmar el nuevo estado en el shadow
        reportState();
    }
}

void Greenhouse::handleCallback(char* topic, byte* payload, unsigned int length) {
    char message[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    
    if (strcmp(topic, deltaTopic) == 0) {
        handleDeltaMessage(message);
    }
}

void Greenhouse::reportState() {
    outputDoc.clear();
    JsonObject state = outputDoc.createNestedObject("state");
    JsonObject reported = state.createNestedObject("reported");

    // Actualiza los valores reportados
    reported["alarmEnabled"] = alarmEnabled;
    reported["alarmActive"] = alarmActive;
    reported["fireDetected"] = flameSensor->checkFireDetected();
    reported["ledState"] = led->getState();    // Obtiene el estado del LED
    reported["buzzerState"] = buzzer->getState();  // Obtiene el estado del Buzzer
    reported["airQualityState"] = airQualitySensor->readPPM() > 900 ? 1 : 0;  // Calidad de aire

    // Serializa el documento JSON y publica el estado al shadow
    serializeJson(outputDoc, outputBuffer);
    mqttClient.publish(updateTopic, outputBuffer);
}

void Greenhouse::setAlarmEnabled(bool enabled) {
    alarmEnabled = enabled;
    alarmActive = enabled; // Si la alarma está habilitada, también debe estar activa

    // Inicia o detén los actuadores según el estado de la alarma
    if (alarmEnabled) {
        Serial.println("Alarma activada.");
    } else {
        Serial.println("Alarma desactivada.");
    }
}

void Greenhouse::publishSensorData(const char* sensorId, float value, const char* unit) {
    if (!mqttClient.connected()) {
        Serial.println("No conectado a MQTT, no se puede publicar.");
        return;
    }

    // Prepara el timestamp
    char timestamp[25];
    time_t now = time(nullptr);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

    // Crear el payload JSON
    StaticJsonDocument<200> sensorDoc;
    sensorDoc["SensorID"] = sensorId;
    sensorDoc["Timestamp"] = timestamp;
    sensorDoc["Value"] = value;
    sensorDoc["Unit"] = unit;

    // Serializa el JSON y publícalo en el tema MQTT
    char buffer[256];
    serializeJson(sensorDoc, buffer);

    if (mqttClient.publish("sensors/data", buffer)) {
        Serial.print("Datos publicados correctamente para ");
        Serial.println(sensorId);
    } else {
        Serial.print("Error publicando datos para ");
        Serial.println(sensorId);
    }
}



void Greenhouse::run() {
    if (!mqttClient.connected()) {
        reconnect("ESP32_SensorPublisher");
    }
    mqttClient.loop();

    unsigned long currentMillis = millis();

    // Verifica si han pasado 5 segundos para publicar datos
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;  // Actualiza el tiempo de la última publicación

        // Leer y reportar estado del sensor de llama
        bool fireDetected = flameSensor->checkFireDetected();
        Serial.print("Sensor de llama detecta fuego: ");
        Serial.println(fireDetected ? "Sí" : "No");
        publishSensorData("FlameSensor", fireDetected ? 1.0f : 0.0f, "");  // Publicar datos del sensor de llama

        // Leer y reportar estado del sensor de calidad de aire
        int airQualityPPM = airQualitySensor->readPPM();
        Serial.print("Calidad de aire (PPM): ");
        Serial.println(airQualityPPM);
        publishSensorData("AirQualitySensor", (float)airQualityPPM, "ppm"); // Publicar datos de calidad de aire

        // Actualizar actuadores según el estado
        updateActuators();
    }
}
