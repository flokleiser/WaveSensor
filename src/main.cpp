// #define ARDUINO_USB_CDC_ON_BOOT 1
#define ARDUINO_USB_MODE 1

#include <M5Unified.h>
#include <Adafruit_NeoPixel.h>

#define PIN 8
#define NUMPIXELS 8

#define DELAYVAL 500

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

float gx = 0.0, gy = -1.0, gz = 0.0;

void setup()
{
    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    M5.begin(cfg);

    M5.Display.setRotation(2);
    M5.Imu.begin();

    M5.Display.setTextSize(2);
    M5.Display.println("IMU Data Reader");
    M5.Display.println("Starting...");

    pixels.begin();
    pixels.setBrightness(50);

    Serial.println("test");

    delay(1000);
}


// uint32_t getColor(float value)
// {
//     value = constrain(value, 0.0, 1.0);

//     int r = (int)(255 * value);
//     int g = (int)(255 * (1.0 - abs(value * 2.0 - 1.0)));
//     int b = (int)(255 * (1.0 - value));

//     return pixels.Color(r, g, b, 0);
// }

uint32_t getColor(float value)
{
    value = constrain(value, 0.0, 1.0);

    int r, g, b;

    if (value < 0.5)
    {
        r = (int)(255 * value * 2);
        g = (int)(255 * (1.0 - value * 2));
        b = 255 - (int)(255 * value * 2);
    }
    else
    {
        r = 255;
        g = 0;
        b = 0;
    }

    return pixels.Color(r, g, b, 0);
}

void loop()
{
    float accX = 0.0F;
    float accY = 0.0F;
    float accZ = 0.0F;

    M5.Imu.getAccel(&accX, &accY, &accZ);

    // from 0 to 1

    float ax = fabs(accX - gx);
    float ay = fabs(accY - gy);
    float az = fabs(accZ - gz);

    // from -1 to 1
    //   float ax = accX - gx;
    //   float ay = accY - gy;
    //   float az = accZ - gz;

    float ayAbs = fabs(ay);
    ayAbs = constrain(ayAbs, 0.0, 1.0);

    int numLeds = (int)(ayAbs * NUMPIXELS);
    uint32_t color = getColor(ayAbs);

    pixels.clear();
    for (int i = 0; i < numLeds; i++)
    {
        pixels.setPixelColor(i, color);
    }
    pixels.show();

    M5.Display.clear();
    M5.Display.setCursor(0, 0);
    M5.Display.printf("TEST:\n");

    M5.Display.printf("X: %.2f\n", ax);
    M5.Display.printf("Y: %.2f\n", ay);
    M5.Display.printf("Z: %.2f\n", az);

    Serial.printf("%.2f, %.2f, %.2f\n", ax, ay, az);

    M5.update();
    delay(100);
}
