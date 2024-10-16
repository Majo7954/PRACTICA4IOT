#include "Invernadero.h"

Invernadero::Invernadero(WiFiClient& espClient, int sensorPin)
    : client(espClient), sensor(sensorPin), fuegoDetectado(false), led(23), buzzer(5) {}

void Invernadero::setupMQTT(const char* server, int port, const char* clientID) {
    client.setServer(server, port);
    if (client.connect(clientID)) {
        Serial.println("Conectado al broker MQTT con éxito.");
        publish("invernadero/estado", "Conectado");
    } else {
        Serial.print("Error de conexión al broker MQTT. Código: ");
        Serial.println(client.state());
    }
}

void Invernadero::setCallback(void (*callback)(char*, byte*, unsigned int)) {
    client.setCallback(callback);
}

void Invernadero::publish(const char* topic, const char* message) {
    if (client.publish(topic, message)) {
        Serial.print("Mensaje enviado al tópico ");
        Serial.print(topic);
        Serial.print(": ");
        Serial.println(message);
    } else {
        Serial.println("Error al enviar mensaje MQTT.");
    }
}

void Invernadero::loop() {
    if (!client.connected()) {
        reconnect("invernadero/estado", "invernadero/control");
    }
    client.loop();
}

void Invernadero::checkFireSensor() {
    bool valor = sensor.leer();
    Serial.print("Estado del sensor de llama: ");
    Serial.println(valor);

    if (valor && !fuegoDetectado) {
        Serial.println("Llama detectada, enviando alerta.");
        publish("invernadero/estado", "FUEGO ALERTA");
        fuegoDetectado = true;
    } else if (!valor && fuegoDetectado) {
        Serial.println("No se detecta llama, enviando actualización.");
        publish("invernadero/estado", "No hay llama");
        fuegoDetectado = false;
    }
}

void Invernadero::controlActuadores(bool estado) {
    if (estado) {
        led.parpadear(100);  // Parpadeo indefinido del LED con 100 ms de intervalo
        buzzer.activar();     // Activa el buzzer
    } else {
        led.apagar();         // Asegura que el LED esté apagado
        buzzer.desactivar();  // Apaga el buzzer
    }
}

bool Invernadero::isConnected() {
    return client.connected();
}

void Invernadero::reconnect(const char* mqttTopic, const char* controlTopic) {
    while (!client.connected()) {
        Serial.print("Intentando reconectar al broker MQTT...");
        if (client.connect("ESP32_Invernadero")) {
            Serial.println("Reconexión exitosa.");
            client.subscribe(controlTopic);
            publish(mqttTopic, "Conectado");
        } else {
            Serial.print("Error de reconexión. Código: ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}
