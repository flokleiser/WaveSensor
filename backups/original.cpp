#include <PL_ADXL355.h> //https://github.com/plasmapper/adxl355-arduino
PL::ADXL355 ADXL355;
#define SPI_CS 1
#define SPI_DRDY 14
#define SPI_SCK 2
#define SPI_MOSI 3
#define SPI_MISO 4

#include <I2S.h> //part of https://github.com/earlephilhower/arduino-pico
I2S i2s(INPUT);
#define I2S_DATA 9
#define I2S_BCLK 10 // WS is BLCK+1
#define I2S_BIT 32
#define sampling_rate 4400 // set around 10% above sampling rate of ADXL355 for jitter free performance

#include <Adafruit_NeoPixel.h> //https://github.com/adafruit/Adafruit_NeoPixel
#define LED_PIN 16
Adafruit_NeoPixel strip(1, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long lastMicros = 0;

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
    ADXL355.setRange(PL::ADXL355_Range::range2g);
    ADXL355.setOutputDataRate(PL::ADXL355_OutputDataRate::odr4000);
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

    // unsigned long now = micros();
    // int sps = 1e6 / (now - lastMicros);
    // lastMicros = now;

    char buf[64];
    int len = snprintf(buf, sizeof(buf), "%d,%d,%d,%d,%d\n", micros(), acc.x, acc.y, acc.z, mic);
    // int len = snprintf(buf, sizeof(buf), "%d,%d,%d,%d,%d\n", sps*10, acc.x, acc.y, acc.z, mic);
    Serial.write(buf, len);
}

void setup1()
{
    i2s.setDATA(I2S_DATA);
    i2s.setBCLK(I2S_BCLK);

    i2s.setBitsPerSample(I2S_BIT);
    i2s.setFrequency(sampling_rate);

    if (!i2s.begin())
    {
        Serial.println("I2S error");
        while (1)
            ;
    }

    strip.begin();
    uint32_t col = strip.ColorHSV(134 * 256, 255, 255);
    strip.fill(col);
    strip.show();
}

void loop1()
{
    // sample I2S mic
    int32_t left, right;

    while (!i2s.available())
        ;
    i2s.read32(&left, &right);

    rp2040.fifo.push(right >> 8);
}