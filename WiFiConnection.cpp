#include "WiFiConnection.h"

WiFiConnection::WiFiConnection(const char* ssid, const char* password) : ssid(ssid), password(password) {}

void WiFiConnection::connect() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

bool WiFiConnection::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}
