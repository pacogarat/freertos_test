#include <Arduino.h>
#include "tasks\WifiTask.h"
#include "tasks\WebSerialWrapper.h"
#include "tasks\OTATask.h"
#ifndef WebSerial
  #include <WebSerial.h>
#endif
#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp

WifiTask* wifi = new WifiTask("MiFibra-4980", "5jXdUfg4");
OTATask* ota = new OTATask();
WebSerialWrapper* webserial = new WebSerialWrapper();
DHTesp dht;

const int dhtPin = 17;


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

void setupDHT11() {
	dht.setup(dhtPin, DHTesp::DHT11);
	WebSerial.println("DHT initiated");
}

bool getTemperature() {
	// Reading temperature for humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  TempAndHumidity newValues = dht.getTempAndHumidity();
	// Check if any reads failed and exit early (to try again).
	if (dht.getStatus() != 0) {
		WebSerial.print("DHT11 error status: ");
    WebSerial.println(dht.getStatusString());
		return false;
	}

  WebSerial.print(" T: ");
  WebSerial.println(newValues.temperature);
  WebSerial.print(" H: "); 
  WebSerial.println(newValues.humidity);
	return true;
}

void setup()
{
  Serial.begin(115200);
  wifi->setup();
  ota->setup();
  webserial->setup(onMessage);
  setupDHT11();
}

void loop()
{
  ota->loop();
  getTemperature();
  delay(5000);
}