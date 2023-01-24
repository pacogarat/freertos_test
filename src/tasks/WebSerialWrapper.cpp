#include "WebSerialWrapper.h"

AsyncWebServer server(80);

void WebSerialWrapper::setup(void (*onMessage)(uint8_t *, size_t)) {
    WebSerial.msgCallback(onMessage);
    WebSerial.begin(&server);

    server.begin() ;
}

