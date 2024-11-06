#ifndef CONFIG_H
#define CONFIG_H

// Visualization Mode
#define SIMPLE_MODE true  // Set to false for advanced mode with Madgwick

// WiFi Configuration
const char* WIFI_SSID = "Lo&Lai_2.4Ghz";
const char* WIFI_PASSWORD = "TaiMing9FA&B";

// WebSocket Configuration
const int WEBSOCKET_PORT = 81;

// IMU Configuration
const int SDA_PIN = 21;
const int SCL_PIN = 22;

// Other Settings
const int STATUS_LED_PIN = 2;
const int SERIAL_BAUD_RATE = 115200;
const int DELAY_MS = 1;

#endif