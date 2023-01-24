#ifndef ESPAsyncWebServer
    #include <ESPAsyncWebServer.h>
#endif
#ifndef WebSerial
    #include <WebSerial.h>
#endif

class WebSerialWrapper {
    public:
        void setup(void (*onMessage)(uint8_t *, size_t));
};