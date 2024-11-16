#ifndef BUZZER_H
#define BUZZER_H

class Buzzer {
private:
    int pin;               // Pin conectado al buzzer
    bool isActive;         // Estado del buzzer (activo o inactivo)
    int frequency;         // Frecuencia actual del buzzer

public:
    Buzzer(int pin);
    void init();
    void setFrequency(int freq); // Cambia la frecuencia
    void turnOn();              // Activa el buzzer
    void turnOff();             // Apaga el buzzer
    bool getState() const { return isActive; }
};

#endif
