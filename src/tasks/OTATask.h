#ifndef WiFi
    #include <WiFi.h>
#endif
#ifndef ArduinoOTA
    #include <ArduinoOTA.h>
#endif

class OTATask {
    public:
        void setup();
        void loop();
};