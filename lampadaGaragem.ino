#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <LiquidCrystal_I2C.h>
#include "CTimer.h"

#ifndef STASSID
#define STASSID "OpenSoftware_2G"
#define STAPSK "santos@info09"

#define STASSID1 "OpenSoftware4"
#define STAPSK1 "santos@info09"

#endif

extern void setupRtc();
extern void loopRtc();
extern void alarm1();
extern void alarm2();
extern void esp8266_eepromInit();

void serverSetup(void);
void serverLoop(void);

const char* ssid = STASSID;
const char* password = STAPSK;
LiquidCrystal_I2C lcd(0x27,16,2);
CTimer ledTimer = CTimer(350);
CTimer wifiConTimer = CTimer(5000);

const int ssidChoose = 12;

void piscaLed()
{
  static bool ledStatus=false;

    if( ledTimer.verifyTimer() ){
      ledTimer.setTimer(350);
      if(ledStatus == false ){
        digitalWrite(LED_BUILTIN, HIGH);
        ledStatus = true;
      }else{
        digitalWrite(LED_BUILTIN, LOW);
        ledStatus = false;
      }
    }
}

void connectWifi()
{
  WiFi.mode(WIFI_STA);
  if( digitalRead( ssidChoose ) == 0){
    lcd.setCursor(0,0);
    lcd.print("OpenSoftware_2G ");
    WiFi.begin(STASSID, STAPSK);
    wifiConTimer.setTimer(5000);
    Serial.println("Trying to connect...");
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Verifique a senha!!\n Verifique o SSID está correto! \nRebooting...");
      if ( wifiConTimer.verifyTimer() ){
        ESP.restart();
      }
    }
  }
  if( digitalRead( ssidChoose ) == 1){
    lcd.setCursor(0,0);
    lcd.print("OpenSoftware4    ");
    WiFi.begin(STASSID1, STAPSK1);
    wifiConTimer.setTimer(5000);
    Serial.println("Trying to connect...");
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Verifique a senha!!\n Verifique o SSID está correto! \nRebooting...");
      if ( wifiConTimer.verifyTimer() ){
        ESP.restart();
      }
    }
  }

}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(74880);
  esp8266_eepromInit();
  pinMode(ssidChoose, INPUT );
  delay(5000);
  
  lcd.init();
	lcd.backlight();

	lcd.setCursor(0,0);
	lcd.print("Lampada Garagem ");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(13, HIGH);
  delay(5000);
  digitalWrite(13,LOW);

 
  Serial.println("Booting");

  connectWifi();


  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("LampadaGaragem");

  // No authentication by default
  ArduinoOTA.setPassword("@lampadaGaragem");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }
    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());

  //iniciando o server
  serverSetup();
  setupRtc();
   loopRtc();
}

// the loop function runs over and over again forever
void loop() {
  ArduinoOTA.handle();
  if( WiFi.waitForConnectResult() != WL_CONNECTED ){
    lcd.setCursor(0,0);
    lcd.print("DESCONECTADO......");
    connectWifi();
  }else{
    piscaLed();
  }
  serverLoop();                     // wait for a second
  alarm1();
  alarm2();
}
