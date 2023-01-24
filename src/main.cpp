#include <Arduino.h>
#include "tasks\WifiTask.h"
#include "tasks\WebSerialWrapper.h"
#include "tasks\OTATask.h"
#ifndef WebSerial
  #include <WebSerial.h>
#endif

WifiTask* wifi = new WifiTask("MiFibra-4980", "5jXdUfg4");
OTATask* ota = new OTATask();
WebSerialWrapper* webserial = new WebSerialWrapper();

void onMessage(uint8_t *data, size_t len)
{
  String comando = "";
  for(int i=0; i < len; i++) {
    comando += char(data[i]);
  }
  comando.toLowerCase();
  WebSerial.println("<<<<<<<<<< Comando >>>>>>>>>>"+ comando);
  if (comando == "on")
  {
  }
  else {
    if (comando=="off") {
      
    } else {
      char option1  = comando.charAt(0);
      comando.remove(0,1);
      switch (option1) {
        case 'x': 
          break;
        default:
          WebSerial.println("Comando: no existe: "+ option1);
          WebSerial.println("Payload: "+ comando);
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);
  wifi->setup();
  ota->setup();
  webserial->setup(onMessage);
}

void loop()
{
  ota->loop();
  delay(100);
}