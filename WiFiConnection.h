#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <WiFi.h>
#include <WiFiClientSecure.h>

class WiFiConnection {
private:
    const char* ssid;
    const char* password;
    WiFiClientSecure wifiClient;
    
public:
    WiFiConnection(const char* ssid, const char* password);
    bool connect();
    bool isConnected();
    WiFiClientSecure* getClient();
    void setupAWSCertificates(const char* rootCA, const char* cert, const char* privateKey);
};
#endif