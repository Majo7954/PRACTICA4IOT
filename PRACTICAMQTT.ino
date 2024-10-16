#include <WiFi.h>
#include <PubSubClient.h>
#include "Invernadero.h"

const char* ssid = "Mi 9T Pro";               // Cambia por tu SSID
const char* password = "boquitapapa";               // Cambia por tu contraseña
const char* mqttServer = "test.mosquitto.org";   // Ejemplo con otro broker
const int mqttPort = 1883;                       // Puerto MQTT
const char* mqttTopic = "invernadero/estado";    // Tema para el estado del sensor
const char* controlTopic = "invernadero/control";// Tema para recibir comandos

const char* clientID = "ESP32_Invernadero";      // Cambia por un Client ID único para el ESP32
bool actuadorActivo = false;                     // Variable para almacenar el estado del actuador

WiFiClient espClient;
Invernadero invernadero(espClient, 18);          // Instancia de la clase Invernadero

void mqttCallback(char* topic, byte* message, unsigned int length);  // Prototipo del callback

void setup() {
    Serial.begin(115200);
  
    // Conexión a Wi-Fi
    Serial.print("Conectando a Wi-Fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConexión Wi-Fi establecida con éxito.");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());

    // Configurar el cliente MQTT a través de la clase Invernadero
    invernadero.setupMQTT(mqttServer, mqttPort, clientID);
    invernadero.setCallback(mqttCallback);  // Función de callback para mensajes recibidos
}

void loop() {
    invernadero.loop();  // Procesa el cliente MQTT y reconecta si es necesario
    invernadero.checkFireSensor();  // Detecta llama usando el objeto sensor

    // Controla los actuadores (LED y Buzzer) basado en el comando recibido desde la app
    invernadero.controlActuadores(actuadorActivo);  // Utiliza el estado de actuadorActivo
}

void mqttCallback(char* topic, byte* message, unsigned int length) {
    String msg;
    for (int i = 0; i < length; i++) {
        msg += (char)message[i];
    }

    Serial.print("Mensaje recibido en el tema: ");
    Serial.print(topic);
    Serial.print(" -> ");
    Serial.println(msg);

    if (String(topic) == controlTopic) {
        if (msg == "ACTIVAR") {
            actuadorActivo = true;
            Serial.println("Actuadores ACTIVADOS.");
        } else if (msg == "DESACTIVAR") {
            actuadorActivo = false;
            Serial.println("Actuadores DESACTIVADOS.");
        }
    }
}
