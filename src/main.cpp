#include <Arduino.h>
#include "tasks\WifiTask.h"
#include "tasks\OTATask.h"
#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#include "PubSubClient.h"
#include "ArduinoJson.h"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60       /* Time ESP32 will go to sleep (in seconds) */
#define TIME_FIRST_ON  120000   /* Time to send commands */

bool first_wakeup;
int loop_count = 0;

WifiTask* wifi = new WifiTask("MiFibra-4980", "5jXdUfg4");
OTATask* ota = new OTATask();
DHTesp dht;

WiFiClient wifiClient;

IPAddress mqtt_server(192, 168, 1, 34);
PubSubClient mqttClient(wifiClient);

const int dhtPin          = 17;
const char *topic_config_t  = "homeassistant/sensor/sensorTemperature/config";
const char *topic_config_h  = "homeassistant/sensor/sensorHumidity/config";
const char *topic_state     = "homeassistant/sensor/sensorTH/state";

int sentValues = 0;
RTC_DATA_ATTR int tempTrigger   = 23;

void publishMqttHumidityDiscover() {
  StaticJsonDocument<195> doc;
  
  doc["device_class"]         = "humidity";
  doc["name"]                 = "HumiditySensor";
  doc["state_topic"]          = topic_state;
  doc["unit_of_measurement"]  = "%";
  doc["value_template"]       = "{{ value_json.humidity}}";

  String message;
  serializeJson(doc, message);
  Serial.print("Enviando con flag retain: ");
  Serial.println(message.c_str());  
  mqttClient.publish(topic_config_h, (uint8_t*) message.c_str(), uint8_t(message.length()), true);
}

void publishMqttTemperatureDiscover() {
  StaticJsonDocument<195> doc;
  
  doc["device_class"]         = "temperature";
  doc["name"]                 = "TemperatureSensor";
  doc["state_topic"]          = topic_state;
  doc["unit_of_measurement"]  = "°C";
  doc["value_template"]       = "{{ value_json.temperature}}";

  String message;
  serializeJson(doc, message);
  Serial.print("Enviando con flag retain: ");
  Serial.println(message.c_str());  
  mqttClient.publish(topic_config_t, (uint8_t*) message.c_str(), uint8_t(message.length()), true);
}

// inicia la comunicacion MQTT
// inicia establece el servidor y el callback al recibir un mensaje
void setupMqtt()
{
  mqttClient.setServer(mqtt_server, 1883);
}

void ConnectMqtt(bool retainFlag = false)
{
    Serial.println("Starting MQTT connection...");
    if (mqttClient.connect("TempSensorClient", "pacogarat", "Pgl@1980"))
    {
        Serial.println("MQTT connected!");
        mqttClient.publish("temp sensor connected","habitacion/domotica");
        publishMqttHumidityDiscover();
        publishMqttTemperatureDiscover();
    }
    else
    {
        Serial.print("Failed MQTT connection, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
    }
}

void HandleMqtt()
{
    if (!mqttClient.connected())
    {
        ConnectMqtt();
    }
    mqttClient.loop();
}

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); first_wakeup = false; break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); first_wakeup = true; break;
  }
}

void setupDHT11() {
	dht.setup(dhtPin, DHTesp::DHT11);
	Serial.println("DHT initiated");
}

bool getTemperature() {
	// Reading temperature for humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  TempAndHumidity newValues = dht.getTempAndHumidity();
	// Check if any reads failed and exit early (to try again).
	if (dht.getStatus() != 0) {
		Serial.print("DHT11 error status: ");
    Serial.println(dht.getStatusString());
		return false;
	}
  String jsonString("{ \"temperature\": ");
  String tempString(newValues.temperature);
  jsonString += tempString;
  jsonString += ", \"humidity\": ";
  String humString(newValues.humidity);
  int str_len = humString.length() + 1; 
  char char_hum_array[str_len];
  humString.toCharArray(char_hum_array, str_len);
  jsonString += humString;
  jsonString += "}";
  int json_len = jsonString.length() + 1; 
  char json_char_array[json_len];
  jsonString.toCharArray(json_char_array, json_len);
  Serial.print("Enviando valores: ");
  Serial.println(jsonString);
  mqttClient.publish(topic_state, json_char_array);
  sentValues++;
  return true;
}

void setupDeepSleep() {
  print_wakeup_reason();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");
}

void setup()
{
  Serial.begin(115200);
  setupMqtt();
  wifi->setup();
  ota->setup();
  setupDHT11();
  setupDeepSleep();
}

void loop()
{
  ota->loop();
  HandleMqtt();
  getTemperature();
  if (!first_wakeup && sentValues>10) {
    Serial.println("Entrando en deep sleep");
    Serial.flush();
    mqttClient.disconnect();
    esp_deep_sleep_start();
  } else {
    delay(2000);
    loop_count++;
    if (loop_count>TIME_FIRST_ON/2000) {
      Serial.println("Entrando en deep sleep");
      mqttClient.disconnect();
      esp_deep_sleep_start();
    }
  }
}