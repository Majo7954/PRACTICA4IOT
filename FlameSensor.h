#ifndef FLAME_SENSOR_H
#define FLAME_SENSOR_H

class FlameSensor {
private:
    int pin;
    bool lastReading;
    unsigned long lastReadTime;
    const unsigned long readInterval;

public:
    FlameSensor(int pin);
    void init();
    bool checkFireDetected();
    bool hasStateChanged();
};

#endif
