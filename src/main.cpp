#define ARDUINO_USB_CDC_ON_BOOT 1
#include <M5Unified.h>

void setup()
{
    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    M5.begin(cfg);

    M5.Imu.begin();

    M5.Display.setTextSize(1.5);
    M5.Display.println("IMU Data Reader");
    M5.Display.println("Starting...");

    Serial.println("test");

    delay(1000);
}

void loop()
{
    float accX = 0.0F;
    float accY = 0.0F;
    float accZ = 0.0F;

    float gyroX = 0.0F;
    float gyroY = 0.0F;
    float gyroZ = 0.0F;

    float pitch = 0.0F;
    float roll = 0.0F;
    float yaw = 0.0F;

    M5.Imu.getAccel(&accX, &accY, &accZ);
    M5.Imu.getGyro(&gyroX, &gyroY, &gyroZ);

    //nice screen
    M5.Display.clear();
    M5.Display.setCursor(0, 0);
    M5.Display.printf("Accel:\n");
    M5.Display.printf("X: %.2f\n", accX);
    M5.Display.printf("Y: %.2f\n", accY);
    M5.Display.printf("Z: %.2f\n", accZ);

    M5.Display.printf("\nGyro:\n");
    M5.Display.printf("X: %.2f\n", gyroX);
    M5.Display.printf("Y: %.2f\n", gyroY);
    M5.Display.printf("Z: %.2f\n", gyroZ);


    // Serial.printf("Accel: X=%.2f Y=%.2f Z=%.2f | \n", accX, accY, accZ);
    //Y will be around 1 when upright
    Serial.printf("%.2f, %.2f, %.2f \n", accX, accY, accZ);

    //we dont really need gyro
    // Serial.printf("Gyro: X=%.2f Y=%.2f Z=%.2f | \n", gyroX, gyroY, gyroZ);

    M5.update();
    delay(100);
}