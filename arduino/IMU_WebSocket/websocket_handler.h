#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#include <WebSocketsServer.h>
#include <ArduinoJson.h>

class WebSocketHandler {
private:
    WebSocketsServer webSocket;
    StaticJsonDocument<400> jsonDoc;

public:
    WebSocketHandler() : webSocket(WEBSOCKET_PORT) {}

    void begin() {
        webSocket.begin();
        webSocket.onEvent([](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
            switch(type) {
                case WStype_DISCONNECTED:
                    Serial.printf("[%u] Disconnected!\n", num);
                    break;
                case WStype_CONNECTED:
                    Serial.printf("[%u] Connected!\n", num);
                    break;
                case WStype_TEXT:
                    if (strcmp((char*)payload, "calibrate") == 0) {
                        Serial.println("Calibration requested");
                    }
                    break;
            }
        });
    }

    void loop() {
        webSocket.loop();
    }

    void broadcastIMUData(const IMUData& data) {
        // Reduce JSON size by limiting decimal places
        jsonDoc.clear();
        
        // Basic orientation data with fewer decimal places
        jsonDoc["pitch"] = String(data.pitch, 2); // 2 decimal places
        jsonDoc["roll"] = String(data.roll, 2);
        jsonDoc["yaw"] = String(data.yaw, 2);

        if (!SIMPLE_MODE) {
            // Additional data with fewer decimal places
            jsonDoc["accX"] = String(data.accX, 4);
            jsonDoc["accY"] = String(data.accY, 4);
            jsonDoc["accZ"] = String(data.accZ, 4);
            jsonDoc["gyrX"] = String(data.gyrX, 4);
            jsonDoc["gyrY"] = String(data.gyrY, 4);
            jsonDoc["gyrZ"] = String(data.gyrZ, 4);
            jsonDoc["magX"] = String(data.magX, 4);
            jsonDoc["magY"] = String(data.magY, 4);
            jsonDoc["magZ"] = String(data.magZ, 4);
        }

        String jsonString;
        serializeJson(jsonDoc, jsonString);
        webSocket.broadcastTXT(jsonString);

        if(DEBUG_MODE) {
            static unsigned long lastPrint = 0;
            if(millis() - lastPrint > 1000) {
                Serial.println("Sending: " + jsonString);
                lastPrint = millis();
            }
        }
    }

    int connectedClients() {
        return webSocket.connectedClients();
    }
};

#endif