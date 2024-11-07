#ifndef BUZZER_H
#define BUZZER_H

class Buzzer {
  private:
    int pin;
    bool isActive;
    const int frequency;

  public:
    Buzzer(int pin);
    void init();
    void turnOn();
    void turnOff();
    void toggle();
    bool getState() const { return isActive; }
};

#endif
