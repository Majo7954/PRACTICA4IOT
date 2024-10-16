#ifndef BUZZER_H
#define BUZZER_H

class Buzzer {
  private:
    int pin;

  public:
    Buzzer(int pin);
    void activar();
    void desactivar();
};

#endif

