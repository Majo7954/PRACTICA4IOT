#ifndef WIFICONNECTION_H
#define WIFICONNECTION_H

#include <WiFi.h>

class WiFiConnection {
  private:
    const char* ssid;
    const char* password;
  
  public:
    WiFiConnection(const char* ssid, const char* password);
    void connect();
    bool isConnected();
};

#endif
