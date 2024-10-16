#ifndef SENSOR_H
#define SENSOR_H

class Sensor {
  private:
    int pin;
    bool estado;

  public:
    Sensor(int pin);
    bool leerSensor();
    bool obtenerEstado();
    void establecerEstado(bool nuevoEstado);
};

#endif
