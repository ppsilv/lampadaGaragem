#ifndef __CTIMER_H__
#define __CTIMER_H__
#include <Arduino.h>
#include <stdint.h>

//extern uint32_t millis();

class CTimer{
public:
    CTimer();  
    CTimer(uint16_t time);  //Cria o timer com o tempo já especificado na criação do objeto
    void setTimer();//set o timeou com o tempo especificado na criação do objeto
    void setTimer(uint32_t time);//set o timeou com o tempo especificado
    bool verifyTimer();          //Verifica se ocorreu timeout

private:
  uint32_t  timeout;    //Contem o valor de millis() + time
  uint16_t  time;       //O tempo que o alarme aguardará para disparar.
};


#endif
