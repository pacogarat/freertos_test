#include "tasks\WifiTask.h"

void reconnectWifi(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.reconnect();
}

WifiTask::WifiTask(char* ssid, char* password) {
  this->ssid      = ssid;
  this->password  = password;
}

void WifiTask::setup()
{
    WiFi.onEvent(reconnectWifi, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
    WiFi.begin(this->ssid, this->password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("Could not initialize WiFi");
        delay(5000);
        ESP.restart();
    }
    Serial.println(WiFi.localIP());  
};
