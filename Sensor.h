#ifndef SENSOR_H
#define SENSOR_H

class Sensor {
  private:
    int pin;
    bool estado;

  public:
    Sensor(int pin);
    bool leer();
    bool getEstado();
    void setEstado(bool nuevoEstado);
};

#endif
