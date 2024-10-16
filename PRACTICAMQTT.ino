#include <WiFi.h>
#include <PubSubClient.h>
#include "Invernadero.h"
#include "ConexionWiFi.h"

const char* ssid = "Mi 9T Pro";
const char* contrasena = "boquitapapa";
const char* servidorMQTT = "test.mosquitto.org";
const int puertoMQTT = 1883;
const char* topicoEstado = "invernadero/estado";
const char* topicoControl = "invernadero/control";

const char* idCliente = "ESP32_Invernadero";
bool actuadorActivo = false;

WiFiClient espCliente;
Invernadero invernadero(espCliente, 18);
ConexionWiFi conexionWiFi(ssid, contrasena);

void callbackMQTT(char* topico, byte* mensaje, unsigned int longitud);

void setup() {
    Serial.begin(115200);

    conexionWiFi.conectar();
    if (conexionWiFi.estaConectado()) {
        Serial.println("Conexión Wi-Fi establecida con éxito.");
        Serial.print("Dirección IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Error al conectar al Wi-Fi.");
    }

    invernadero.configurarMQTT(servidorMQTT, puertoMQTT, idCliente);
    invernadero.establecerCallback(callbackMQTT);
}

void loop() {
    invernadero.ejecutar();
    invernadero.verificarSensorFuego();

    invernadero.controlarActuadores(actuadorActivo);
}

void callbackMQTT(char* topico, byte* mensaje, unsigned int longitud) {
    String msg;
    for (int i = 0; i < longitud; i++) {
        msg += (char)mensaje[i];
    }

    Serial.print("Mensaje recibido en el tema: ");
    Serial.print(topico);
    Serial.print(" -> ");
    Serial.println(msg);

    if (String(topico) == topicoControl) {
        if (msg == "ACTIVAR") {
            actuadorActivo = true;
            Serial.println("Actuadores ACTIVADOS.");
        } else if (msg == "DESACTIVAR") {
            actuadorActivo = false;
            Serial.println("Actuadores DESACTIVADOS.");
        }
    }
}
