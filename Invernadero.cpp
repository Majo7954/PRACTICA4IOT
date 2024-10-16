#include "Invernadero.h"

Invernadero::Invernadero(WiFiClient& clienteWiFi, int pinSensor)
    : cliente(clienteWiFi), sensor(pinSensor), fuegoDetectado(false), led(23), buzzer(5) {}

void Invernadero::configurarMQTT(const char* servidor, int puerto, const char* idCliente) {
    cliente.setServer(servidor, puerto);
    if (cliente.connect(idCliente)) {
        Serial.println("Conectado al broker MQTT con éxito.");
        publicar("invernadero/estado", "Conectado");
    } else {
        Serial.print("Error de conexión al broker MQTT. Código: ");
        Serial.println(cliente.state());
    }
}

void Invernadero::establecerCallback(void (*callback)(char*, byte*, unsigned int)) {
    cliente.setCallback(callback);
}

void Invernadero::publicar(const char* topico, const char* mensaje) {
    if (cliente.publish(topico, mensaje)) {
        Serial.print("Mensaje enviado al tópico ");
        Serial.print(topico);
        Serial.print(": ");
        Serial.println(mensaje);
    } else {
        Serial.println("Error al enviar mensaje MQTT.");
    }
}

void Invernadero::ejecutar() {
    if (!cliente.connected()) {
        reconectar("invernadero/estado", "invernadero/control");
    }
    cliente.loop();
}

void Invernadero::verificarSensorFuego() {
    bool valor = sensor.leerSensor();
    Serial.print("Estado del sensor de llama: ");
    Serial.println(valor);

    if (valor && !fuegoDetectado) {
        Serial.println("Llama detectada, enviando alerta.");
        publicar("invernadero/estado", "FUEGO ALERTA");
        fuegoDetectado = true;
    } else if (!valor && fuegoDetectado) {
        Serial.println("No se detecta llama, enviando actualización.");
        publicar("invernadero/estado", "No hay llama");
        fuegoDetectado = false;
    }
}

void Invernadero::controlarActuadores(bool estado) {
    if (estado) {
        led.parpadear(100);
        buzzer.activar();
    } else {
        led.apagar();
        buzzer.desactivar();
    }
}

bool Invernadero::estaConectado() {
    return cliente.connected();
}

void Invernadero::reconectar(const char* topicoMQTT, const char* topicoControl) {
    while (!cliente.connected()) {
        Serial.print("Intentando reconectar al broker MQTT...");
        if (cliente.connect("ESP32_Invernadero")) {
            Serial.println("Reconexión exitosa.");
            cliente.subscribe(topicoControl);
            publicar(topicoMQTT, "Conectado");
        } else {
            Serial.print("Error de reconexión. Código: ");
            Serial.println(cliente.state());
            delay(2000);
        }
    }
}
