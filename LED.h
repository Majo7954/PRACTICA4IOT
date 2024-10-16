#ifndef LED_H
#define LED_H

class LED {
  private:
    int pin;
    unsigned long tiempoAnterior;
    bool estadoLED;
    
  public:
    LED(int pin);
    void encender();
    void apagar();
    void parpadear(int tiempo);
};

#endif
