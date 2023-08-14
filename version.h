#ifndef __CVERSION_H__
#define __CVERSION_H__
#include <EEPROM.h>
#include <stdint.h>
#define SERIAL_NUMBER     "20230811"
#define MCU "ESP"
#define VERSION_SIZE  0x10

char version[VERSION_SIZE+2];

const uint8_t mes[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
void rtcGet();
extern int getDay();
extern int getMonth();
extern String getFormatedDate();
extern String getFormatedTime();


void esp8266_eepromInit()
{
  EEPROM.begin(VERSION_SIZE+1);  
}

uint16_t getWeekNumber()
{
    int i=0;
    rtcGet();
    uint16_t weekNumber = 0;
    Serial.print("getMonth() ["); Serial.print(getMonth()); Serial.println("]");
    for(i=1; i < getMonth(); i++){
        weekNumber +=  mes[i];
    }
    weekNumber += getDay();
    
    Serial.print("weekNumber ["); Serial.print(weekNumber); Serial.println("]");
    return weekNumber;
}

char * getVersion()
{
  //apaga eeprom se a chave foi pressionada...
  /*
  Serial.print("getVersion: Tecla eraseEprom (");
  Serial.print(eraseEprom);
  Serial.println(")");

  if( eraseEprom == 0 ){
    Serial.println("Tecla eraseEprom Apagando a eeprom...");
    for (int i = 0 ; i < VERSION_SIZE; i++) {
       EEPROM.write(i, 0xFF);
    }
  }
*/

  uint8_t eepromByte0;
  EEPROM.get(0,eepromByte0);
  getWeekNumber();
  if( eepromByte0 == 0xFF){
    sprintf((char *) version, "%s-%04d%s", SERIAL_NUMBER, getWeekNumber(),MCU );
	  Serial.println("Programming EEProm..");
	  Serial.println((char *)version);

    for (int i=0, j=0 ; i < VERSION_SIZE ; i++,j++) {
      EEPROM.put(i, version[j]);
    }
  }else{
    Serial.println("Reading EEProm..");
    memset(version,0,sizeof(version));
    for (int i=0, j=0 ; i < VERSION_SIZE ; i++,j++) {
      EEPROM.get(i,version[j]);
    } 

    Serial.print("Version..: [");
	  Serial.print((char *)version);
    Serial.println("]");
  }
  return version;
}

#endif
