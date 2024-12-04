#ifndef AIR_QUALITY_SENSOR_H
#define AIR_QUALITY_SENSOR_H

class AirQualitySensor {
private:
    int pin;             // Pin del sensor
    int threshold;       // Umbral para calidad de aire
    int lastReading;     // Última lectura

public:
    AirQualitySensor(int pin, int threshold = 900);
    void init();
    int readPPM();       // Devuelve 1 si supera el umbral, 0 en caso contrario
    int getLastReading() const; // Devuelve la última lectura del sensor
};

#endif