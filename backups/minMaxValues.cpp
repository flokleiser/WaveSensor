#include <PL_ADXL355.h>
PL::ADXL355 ADXL355;
#define SPI_CS 1
#define SPI_DRDY 14
#define SPI_SCK 2
#define SPI_MOSI 3
#define SPI_MISO 4
#include <Adafruit_NeoPixel.h>
#define LED_PIN 16
Adafruit_NeoPixel strip(1, LED_PIN, NEO_GRB + NEO_KHZ800);

struct CalibrationData
{
    int32_t x_min = INT32_MAX;
    int32_t x_max = INT32_MIN;
    int32_t y_min = INT32_MAX;
    int32_t y_max = INT32_MIN;
    int32_t z_min = INT32_MAX;
    int32_t z_max = INT32_MIN;
    uint32_t sample_count = 0;
    bool calibrating = true;
};

CalibrationData cal;
unsigned long calibration_start = 0;
const unsigned long CALIBRATION_TIME_MS = 30000;

void updateCalibration(int32_t x, int32_t y, int32_t z)
{
    if (x < cal.x_min)
        cal.x_min = x;
    if (x > cal.x_max)
        cal.x_max = x;
    if (y < cal.y_min)
        cal.y_min = y;
    if (y > cal.y_max)
        cal.y_max = y;
    if (z < cal.z_min)
        cal.z_min = z;
    if (z > cal.z_max)
        cal.z_max = z;
    cal.sample_count++;
}

int mapToRange(int32_t value, int32_t old_min, int32_t old_max, int new_min, int new_max)
{
    if (old_max == old_min)
        return 0;
    return new_min + ((long)(value - old_min) * (new_max - new_min)) / (old_max - old_min);
}

void printCalibrationResults()
{
    Serial.println("Calibration complete!");
    Serial.print("X range: ");
    Serial.print(cal.x_min);
    Serial.print(" to ");
    Serial.println(cal.x_max);
    Serial.print("Y range: ");
    Serial.print(cal.y_min);
    Serial.print(" to ");
    Serial.println(cal.y_max);
    Serial.print("Z range: ");
    Serial.print(cal.z_min);
    Serial.print(" to ");
    Serial.println(cal.z_max);
    Serial.print("Samples collected: ");
    Serial.println(cal.sample_count);
    Serial.println("Now outputting mapped values (-127 to 127)");
}

void setup()
{
    Serial.begin(2000000);
    while (!Serial)
        ;

    pinMode(SPI_DRDY, INPUT_PULLUP);
    SPI.setSCK(SPI_SCK);
    SPI.setTX(SPI_MOSI);
    SPI.setRX(SPI_MISO);
    SPI.begin();
    ADXL355.beginSPI(SPI_CS);
    ADXL355.setRange(PL::ADXL355_Range::range4g);
    ADXL355.setOutputDataRate(PL::ADXL355_OutputDataRate::odr125);
    ADXL355.enableDataReady();
    ADXL355.enableMeasurement();

    strip.begin();
    strip.setPixelColor(0, strip.Color(255, 0, 0));
    strip.show();

    calibration_start = millis();
    Serial.println("Starting 30-second calibration. Rotate sensor through all orientations!");
}

void loop()
{
    uint32_t mic = UINT32_MAX;
    rp2040.fifo.pop_nb(&mic);

    while (digitalRead(SPI_DRDY) == LOW)
    {
    }

    auto acc = ADXL355.getRawAccelerations();

    if (cal.calibrating)
    {
        updateCalibration(acc.x, acc.y, acc.z);

        if (millis() - calibration_start >= CALIBRATION_TIME_MS)
        {
            cal.calibrating = false;
            printCalibrationResults();
            strip.setPixelColor(0, strip.Color(0, 255, 0));
            strip.show();
        }
        else
        {
            unsigned long remaining = (CALIBRATION_TIME_MS - (millis() - calibration_start)) / 1000;
            if (cal.sample_count % 125 == 0)
            {
                Serial.print("Calibrating... ");
                Serial.print(remaining);
                Serial.println(" seconds remaining");
            }
        }

        char buf[64];
        int len = snprintf(buf, sizeof(buf), "%d,%d,%d\n", acc.x, acc.y, acc.z);
        Serial.write(buf, len);
    }
    else
    {
        // int mapped_x = mapToRange(acc.x, cal.x_min, cal.x_max, -127, 127);
        // int mapped_y = mapToRange(acc.y, cal.y_min, cal.y_max, -127, 127);
        // int mapped_z = mapToRange(acc.z, cal.z_min, cal.z_max, -127, 127);
        int mapped_x = mapToRange(acc.x, cal.x_min, cal.x_max, -1, 1);
        int mapped_y = mapToRange(acc.y, cal.y_min, cal.y_max, -1, 1);
        int mapped_z = mapToRange(acc.z, cal.z_min, cal.z_max, -1, 1);

        char buf[64];
        // int len = snprintf(buf, sizeof(buf), "%d,%d,%d\n", mapped_x, mapped_y, mapped_z);
        int len = snprintf(buf, sizeof(buf), "%d,%d,%d,%d,%d,%d\n", cal.x_min, cal.x_max, cal.y_min, cal.y_max, cal.z_min, cal.z_max);
        Serial.write(buf, len);
    }
}