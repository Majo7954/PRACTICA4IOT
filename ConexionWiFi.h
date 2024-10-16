#ifndef CONEXIONWIFI_H
#define CONEXIONWIFI_H

#include <WiFi.h>

class ConexionWiFi {
  private:
    const char* ssid;
    const char* contrasena;
  
  public:
    ConexionWiFi(const char* ssid, const char* contrasena);
    void conectar();
    bool estaConectado();
};

#endif
