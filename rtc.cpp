#include <NTPClient.h> //NTPclient Lib: https://github.com/SanUSB/NTPClient 
#include <WiFiUdp.h>
#include <time.h>

extern void ligaLampada();
extern void desligaLampada();

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
#define led 2

struct tm *timeinfo=nullptr;
time_t epochTime;

void setupRtc() {
// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(-3 * 3600);
  timeClient.update();
}

void loopRtc() {
  

  epochTime = timeClient.getEpochTime();
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  
  String formattedTime = timeClient.getFormattedTime();
  Serial.print("Formatted Time: ");
  Serial.println(formattedTime);  
  if (formattedTime == "22:29:50")
   {
      Serial.println("Alarm!!!");
   }

  int currentHour = timeClient.getHours();
  Serial.print("Hour: ");
  Serial.println(currentHour);  

  int currentMinute = timeClient.getMinutes();
  Serial.print("Minutes: ");
  Serial.println(currentMinute); 
   
  int currentSecond = timeClient.getSeconds();
  Serial.print("Seconds: ");
  Serial.println(currentSecond);  
//********************************************************************************************
  //Get a time structure
  timeinfo = gmtime ((time_t *)&epochTime); 

  int monthDay = timeinfo->tm_mday;
  Serial.print("day: ");
  Serial.println(monthDay);

  int currentMonth = timeinfo->tm_mon+1;
  Serial.print("Month: ");
  Serial.println(currentMonth);

  int currentYear = timeinfo->tm_year;//+1900;
  Serial.print("Year: ");
  Serial.println(currentYear);

  //Print complete date:
  String currentDate = String(monthDay) + "/" + String(currentMonth) + "/" + String(currentYear);
  Serial.print("Current date: ");
  Serial.println(currentDate);

  Serial.println("");

  delay(1000);
}

void rtcGet()
{
  timeClient.update();
  epochTime = timeClient.getEpochTime();
  timeinfo = gmtime ( &epochTime ); 
}
int getDay()
{
  return(timeinfo->tm_mday);
}
int getMonth()
{
  return (timeinfo->tm_mon+1);
}
String getFormatedDate()
{
  
  int ano = timeinfo->tm_year;
  Serial.print("Ano  :  ");
  Serial.println(timeinfo->tm_year);
  Serial.print("Ano  :  ");
  Serial.println(ano);
  
  return (String(timeinfo->tm_mday) + "/" + String(timeinfo->tm_mon+1) + "/" + String(ano+1900));
}
String getFormatedTime()
{
  return   (timeClient.getFormattedTime());
}
void alarm1()
{
  String formattedTime = timeClient.getFormattedTime();
 // Serial.print("Formatted Time: ");
 // Serial.println(formattedTime);  
  if (formattedTime == "05:45:00")
   {
      Serial.println("Liga lampada...");
      Serial.print("Formatted Time: ");
      Serial.println(formattedTime);  
      ligaLampada();
   }  
}

void alarm2()
{
  String formattedTime = timeClient.getFormattedTime();
  if (formattedTime == "06:10:00")
   {
      Serial.println("Desliga lampada...");
      Serial.print("Formatted Time: ");
      Serial.println(formattedTime);  
      desligaLampada();
   }  
}

