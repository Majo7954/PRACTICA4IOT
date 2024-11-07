#include "WiFiConnection.h"

WiFiConnection::WiFiConnection(const char* ssid, const char* password) 
    : ssid(ssid), password(password) {}

bool WiFiConnection::connect() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi");
        return true;
    }
    return false;
}

bool WiFiConnection::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

WiFiClientSecure* WiFiConnection::getClient() {
    return &wifiClient;
}

void WiFiConnection::setupAWSCertificates(const char* rootCA, const char* cert, const char* privateKey) {
    wifiClient.setCACert(rootCA);
    wifiClient.setCertificate(cert);
    wifiClient.setPrivateKey(privateKey);
}