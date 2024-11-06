#define DEBUG_MODE true

#include "config.h"
#include "imu_handler.h"
#include "websocket_handler.h"
#include <WiFi.h>

IMUHandler imuHandler;
WebSocketHandler webSocketHandler;

void printIMUData(const IMUData& data) {
    Serial.println("\n=== IMU Data ===");
    Serial.printf("Accelerometer: X=%.6f Y=%.6f Z=%.6f\n", data.accX, data.accY, data.accZ);
    Serial.printf("Gyroscope: X=%.6f Y=%.6f Z=%.6f\n", data.gyrX, data.gyrY, data.gyrZ);
    Serial.printf("Magnetometer: X=%.6f Y=%.6f Z=%.6f\n", data.magX, data.magY, data.magZ);
    Serial.println("===============");
}

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(1000);
    
    pinMode(STATUS_LED_PIN, OUTPUT);
    
    Serial.println("\n\n=== ESP32 IMU Visualization ===");
    
    // Setup WiFi
    setupWiFi();
    
    // Setup IMU
    Serial.println("Initializing IMU...");
    if (!imuHandler.begin()) {
        Serial.println("IMU initialization failed! Check connections.");
        while(1) {
            digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
            delay(500);
        }
    }
    Serial.println("IMU initialization successful!");
    
    // Setup WebSocket
    webSocketHandler.begin();
    Serial.println("WebSocket server started");
    
    Serial.println("Setup complete!");
}

void loop() {
    webSocketHandler.loop();
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected! Reconnecting...");
        setupWiFi();
        return;
    }
    
    // Update IMU
    imuHandler.update();
    
    // Get IMU data
    IMUData data = imuHandler.getData();
    
    // Print IMU data periodically
    static unsigned long lastIMUPrint = 0;
    if (millis() - lastIMUPrint > 1000) {
        printIMUData(data);
        lastIMUPrint = millis();
    }
    
    // Broadcast data
    webSocketHandler.broadcastIMUData(data);
    
    // Print debug info periodically
    static unsigned long lastDebugPrint = 0;
    if (millis() - lastDebugPrint > 1000) {
        printSystemStatus();
        lastDebugPrint = millis();
    }
    
    delay(DELAY_MS);
}

void setupWiFi() {
    Serial.println("\nConnecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        digitalWrite(STATUS_LED_PIN, !digitalRead(STATUS_LED_PIN));
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        digitalWrite(STATUS_LED_PIN, HIGH);
        Serial.println("\n=== WiFi Connected ===");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.print("MAC Address: ");
        Serial.println(WiFi.macAddress());
        Serial.println("====================\n");
    } else {
        digitalWrite(STATUS_LED_PIN, LOW);
        Serial.println("\nWiFi connection failed!");
    }
}

void printSystemStatus() {
    Serial.println("\n=== System Status ===");
    Serial.printf("WiFi Status: %s\n", WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Signal Strength: %d dBm\n", WiFi.RSSI());
    Serial.printf("Connected Clients: %d\n", webSocketHandler.connectedClients());
    Serial.println("==================\n");
}