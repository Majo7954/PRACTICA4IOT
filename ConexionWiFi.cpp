#include "ConexionWiFi.h"

ConexionWiFi::ConexionWiFi(const char* ssid, const char* contrasena) : ssid(ssid), contrasena(contrasena) {}

void ConexionWiFi::conectar() {
    WiFi.begin(ssid, contrasena);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

bool ConexionWiFi::estaConectado() {
    return WiFi.status() == WL_CONNECTED;
}
