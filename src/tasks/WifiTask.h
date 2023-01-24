#ifndef WiFi
     #include <WiFi.h>
#endif

class WifiTask {
    public:
        WifiTask(char *ssid, char *password);
        void setup();

    private:
        char *ssid;
        char *password;
};