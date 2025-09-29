//CODE BY LUCA SOMM

#include <PL_ADXL355.h> //https://github.com/plasmapper/adxl355-arduino
PL::ADXL355 ADXL355;
#define SPI_CS 1    // chip select
#define SPI_DRDY 14 // data ready
#define SPI_SCK 2   // clock
#define SPI_MOSI 3  // arduino -> sensor data
#define SPI_MISO 4  // sensor -> arduino data

#include <Adafruit_NeoPixel.h> //https://github.com/adafruit/Adafruit_NeoPixel
#define LED_PIN 16
Adafruit_NeoPixel strip(1, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long lastMicros = 0;

void setup()
{
    //   Serial.begin(2000000);
    Serial.begin(9600);
    while (!Serial)
        ;

    pinMode(SPI_DRDY, INPUT_PULLUP);

    SPI.setSCK(SPI_SCK);
    SPI.setTX(SPI_MOSI);
    SPI.setRX(SPI_MISO);
    SPI.begin();

    ADXL355.beginSPI(SPI_CS);

    // this can also be changed
    ADXL355.setRange(PL::ADXL355_Range::range2g);
    //   ADXL355.setRange(PL::ADXL355_Range::range4g);

    ADXL355.setOutputDataRate(PL::ADXL355_OutputDataRate::odr4000);
    //   ADXL355.setOutputDataRate(PL::ADXL355_OutputDataRate::odr125);

    ADXL355.enableDataReady();
    ADXL355.enableMeasurement();
}

void loop()
{
    uint32_t mic = UINT32_MAX;

    rp2040.fifo.pop_nb(&mic);

    while (digitalRead(SPI_DRDY) == LOW)
    {
    }

    auto acc = ADXL355.getRawAccelerations();
    // auto acc = ADXL355.getAccelerations();

    // i just took the absolute maximum number and mapped it all
    int mapped_x = map(acc.x, -524288, 524288, -1000, 1000);
    int mapped_y = map(acc.y, -524288, 524288, -1000, 1000);
    int mapped_z = map(acc.z, -524288, 524288, -1000, 1000);

    char buf[64];
    //   int len = snprintf(buf, sizeof(buf), "%d,%d,%d\n", acc.x, acc.y, acc.z);
    int len = snprintf(buf, sizeof(buf), "%d,%d,%d\n", mapped_x, mapped_y, mapped_z);
    Serial.write(buf, len);
}