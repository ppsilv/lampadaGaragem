#include "CTimer.h"

CTimer::CTimer()
{
}  
CTimer::CTimer(uint16_t time)
{
  this->time = time;
  this->timeout = millis() + this->time;
}  
//Cria o timer com o tempo especificado
void CTimer::setTimer()
{
  
  this->timeout = millis() + this->time;
}
void CTimer::setTimer(uint32_t time)
{
  this->time = time;
  this->timeout = millis() + this->time;
}
//Verifica se ocorreu timeout
bool CTimer::verifyTimer()
{
    if( millis() > this->timeout){
      return true;
    }
    return false;
}
