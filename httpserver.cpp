#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "version.h"

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

const int lampada = 13;
bool  statusLampada = false;
extern void rtcGet();
extern String getFormatedDate();
extern String getFormatedTime();
extern char * getVersion();

String convertToString(char* a, int size)
{
    int i;
    String s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

void showPagina()
{
  String message ;
  String version =  "Versao: ";
  char * ver = getVersion();
  String str;
            
            str = convertToString(ver, 16);
            rtcGet();
            message +="<!DOCTYPE html><html>" ;
            message +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" ;
            message +="<link rel=\"icon\" href=\"data:,\">" ;
            message +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}" ;
            message +=".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;" ;
            message +="text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}" ;
            message +=".button2 {background-color: #77878A;}</style></head>" ;
            message +="<body><h1>L&acirc;mpada da Frente - Web Server</h1>" ;
            message +="<h2>" +  getFormatedDate() + " <-> "+ getFormatedTime() + "</h2>";
            message +="<h2>" +  version + " "+ str + "</h2>";
            
            if ( statusLampada == false ) {
              message +="<p>GPIO 12 - State OFF</p>" ;
              message +="<p><a href=\"/liga\"><button class=\"button\">ON</button></a></p>" ;
            } else {
              message +="<p>GPIO 15 - State ON</p>" ;
              message +="<p><a href=\"/desliga\"><button class=\"button button2\">OFF</button></a></p>" ;
            } 
            message +="</body></html>" ;

  server.send(200, "text/html", message);
}


void ligaLampada(){
  if( statusLampada ){
    showPagina();
  }else{
    digitalWrite(lampada, 1);
    statusLampada = true;
    showPagina();
  }
}

void desligaLampada(){
  if( statusLampada ){
    digitalWrite(lampada, 0);
    statusLampada = false;
    showPagina();
  }else{
    showPagina();
  }
}

void handleRoot() {
  digitalWrite(led, 1);
  showPagina();
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void serverSetup(void) {
  pinMode(lampada, OUTPUT);
  digitalWrite(led, 0);

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  server.on("/", handleRoot);
  server.on("/liga", ligaLampada);
  server.on("/desliga", desligaLampada);



  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/gif", []() {
    static const uint8_t gif[] PROGMEM = {
      0x47, 0x49, 0x46, 0x38, 0x37, 0x61, 0x10, 0x00, 0x10, 0x00, 0x80, 0x01,
      0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x2c, 0x00, 0x00, 0x00, 0x00,
      0x10, 0x00, 0x10, 0x00, 0x00, 0x02, 0x19, 0x8c, 0x8f, 0xa9, 0xcb, 0x9d,
      0x00, 0x5f, 0x74, 0xb4, 0x56, 0xb0, 0xb0, 0xd2, 0xf2, 0x35, 0x1e, 0x4c,
      0x0c, 0x24, 0x5a, 0xe6, 0x89, 0xa6, 0x4d, 0x01, 0x00, 0x3b
    };
    char gif_colored[sizeof(gif)];
    memcpy_P(gif_colored, gif, sizeof(gif));
    // Set the background to a random set of colors
    gif_colored[16] = millis() % 256;
    gif_colored[17] = millis() % 256;
    gif_colored[18] = millis() % 256;
    server.send(200, "image/gif", gif_colored, sizeof(gif_colored));
  });

  server.onNotFound(handleNotFound);

  /////////////////////////////////////////////////////////
  // Hook examples

  server.addHook([](const String& method, const String& url, WiFiClient* client, ESP8266WebServer::ContentTypeFunction contentType) {
    (void)method;       // GET, PUT, ...
    (void)url;          // example: /root/myfile.html
    (void)client;       // the webserver tcp client connection
    (void)contentType;  // contentType(".html") => "text/html"
    Serial.printf("A useless web hook has passed\n");
    Serial.printf("(this hook is in 0x%08x area (401x=IRAM 402x=FLASH))\n", esp_get_program_counter());
    return ESP8266WebServer::CLIENT_REQUEST_CAN_CONTINUE;
  });

  server.addHook([](const String&, const String& url, WiFiClient*, ESP8266WebServer::ContentTypeFunction) {
    if (url.startsWith("/fail")) {
      Serial.printf("An always failing web hook has been triggered\n");
      return ESP8266WebServer::CLIENT_MUST_STOP;
    }
    return ESP8266WebServer::CLIENT_REQUEST_CAN_CONTINUE;
  });

  server.addHook([](const String&, const String& url, WiFiClient* client, ESP8266WebServer::ContentTypeFunction) {
    if (url.startsWith("/dump")) {
      Serial.printf("The dumper web hook is on the run\n");

      // Here the request is not interpreted, so we cannot for sure
      // swallow the exact amount matching the full request+content,
      // hence the tcp connection cannot be handled anymore by the
      // webserver.
#ifdef STREAMSEND_API
      // we are lucky
      client->sendAll(Serial, 500);
#else
      auto last = millis();
      while ((millis() - last) < 500) {
        char buf[32];
        size_t len = client->read((uint8_t*)buf, sizeof(buf));
        if (len > 0) {
          Serial.printf("(<%d> chars)", (int)len);
          Serial.write(buf, len);
          last = millis();
        }
      }
#endif
      // Two choices: return MUST STOP and webserver will close it
      //                       (we already have the example with '/fail' hook)
      // or                  IS GIVEN and webserver will forget it
      // trying with IS GIVEN and storing it on a dumb WiFiClient.
      // check the client connection: it should not immediately be closed
      // (make another '/dump' one to close the first)
      Serial.printf("\nTelling server to forget this connection\n");
      static WiFiClient forgetme = *client;  // stop previous one if present and transfer client refcounter
      return ESP8266WebServer::CLIENT_IS_GIVEN;
    }
    return ESP8266WebServer::CLIENT_REQUEST_CAN_CONTINUE;
  });

  // Hook examples
  /////////////////////////////////////////////////////////

  server.begin();
  Serial.println("HTTP server started");
}

void serverLoop(void) {
  server.handleClient();
  MDNS.update();
}
