#ifndef IMU_HANDLER_H
#define IMU_HANDLER_H

#include <Wire.h>
#include <ICM20948_WE.h>

#define ICM20948_ADDR 0x68

struct IMUData {
    // Basic orientation data
    float pitch = 0;
    float roll = 0;
    float yaw = 0;
    
    // Raw sensor data (for advanced mode)
    float accX = 0, accY = 0, accZ = 0;
    float gyrX = 0, gyrY = 0, gyrZ = 0;
    float magX = 0, magY = 0, magZ = 0;
};

class IMUHandler {
private:
    ICM20948_WE myIMU;
    IMUData data;
    const float alpha = 0.3; // Smoothing factor

    void calculateOrientation(float ax, float ay, float az, float gx, float gy, float gz) {
        // Simple complementary filter for orientation
        float pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
        float roll = atan2(ay, az) * 180.0 / PI;
        
        // Integrate gyroscope data for yaw
        static float yaw = 0;
        static unsigned long lastUpdate = 0;
        unsigned long now = millis();
        float dt = (now - lastUpdate) / 1000.0;
        lastUpdate = now;
        
        yaw += gz * dt;
        
        // Apply smoothing
        data.pitch = (alpha * pitch) + ((1-alpha) * data.pitch);
        data.roll = (alpha * roll) + ((1-alpha) * data.roll);
        data.yaw = (alpha * yaw) + ((1-alpha) * data.yaw);
    }

public:
    IMUHandler() : myIMU(ICM20948_ADDR) {}

    bool begin() {
        Wire.begin(SDA_PIN, SCL_PIN);
        if(!myIMU.init()) {
            Serial.println("ICM20948 initialization failed!");
            return false;
        }
        
        // Configure IMU
        myIMU.setAccRange(ICM20948_ACC_RANGE_4G);
        myIMU.setAccSampleRateDivider(5);
        myIMU.setGyrRange(ICM20948_GYRO_RANGE_250);
        myIMU.setMagOpMode(AK09916_CONT_MODE_20HZ);
        
        // Initial calibration
        calibrate();
        
        Serial.println("ICM20948 initialized successfully!");
        return true;
    }

    void calibrate() {
        Serial.println("Calibrating IMU...");
        myIMU.autoOffsets();
        delay(1000);
        Serial.println("Calibration complete!");
    }

    void update() {
        myIMU.readSensor();
        
        // Get raw sensor data
        xyzFloat acc = myIMU.getGValues();
        xyzFloat gyr = myIMU.getGyrValues();
        xyzFloat mag = myIMU.getMagValues();
        
        // Store raw data (for advanced mode)
        data.accX = acc.x;
        data.accY = acc.y;
        data.accZ = acc.z;
        data.gyrX = gyr.x;
        data.gyrY = gyr.y;
        data.gyrZ = gyr.z;
        data.magX = mag.x;
        data.magY = mag.y;
        data.magZ = mag.z;

        // Calculate orientation
        calculateOrientation(acc.x, acc.y, acc.z, gyr.x, gyr.y, gyr.z);

        // Debug print
        if(DEBUG_MODE) {
            static unsigned long lastPrint = 0;
            if(millis() - lastPrint > 1000) {
                Serial.printf("Orientation - Pitch: %.2f, Roll: %.2f, Yaw: %.2f\n", 
                    data.pitch, data.roll, data.yaw);
                if(!SIMPLE_MODE) {
                    Serial.printf("Acc: X=%.2f Y=%.2f Z=%.2f\n", data.accX, data.accY, data.accZ);
                    Serial.printf("Gyr: X=%.2f Y=%.2f Z=%.2f\n", data.gyrX, data.gyrY, data.gyrZ);
                    Serial.printf("Mag: X=%.2f Y=%.2f Z=%.2f\n", data.magX, data.magY, data.magZ);
                }
                lastPrint = millis();
            }
        }
    }

    IMUData getData() {
        return data;
    }
};

#endif