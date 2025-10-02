// CODE BY LUCA SOMM

#include <PL_ADXL355.h> //https://github.com/plasmapper/adxl355-arduino
PL::ADXL355 ADXL355;
#define SPI_CS 1    // chip select
#define SPI_DRDY 14 // data ready
#define SPI_SCK 2   // clock
#define SPI_MOSI 3  // arduino -> sensor data
#define SPI_MISO 4  // sensor -> arduino data

#include <I2S.h> //part of https://github.com/earlephilhower/arduino-pico
I2S i2s(INPUT);
#define I2S_DATA 9
#define I2S_BCLK 10 // WS is BLCK+1
#define I2S_BIT 32
#define sampling_rate 4400 // ~ 10% above sampling rate of ADXL355

#include <Adafruit_NeoPixel.h> //https://github.com/adafruit/Adafruit_NeoPixel
#define LED_PIN 16
Adafruit_NeoPixel strip(1, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long lastMicros = 0;

void setup() {
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
  // this can also be changed

  // ADXL355.setOutputDataRate(PL::ADXL355_OutputDataRate::odr4000);
  ADXL355.setOutputDataRate(PL::ADXL355_OutputDataRate::odr125);

  ADXL355.enableDataReady();
  ADXL355.enableMeasurement();
}

void loop() {
  uint32_t mic = UINT32_MAX;

  rp2040.fifo.pop_nb(&mic);

  while (digitalRead(SPI_DRDY) == LOW) {
  }

  auto acc = ADXL355.getRawAccelerations();
  // auto acc = ADXL355.getAccelerations();

  // float x_g = (float)acc.x / 262144.0;
  // float y_g = (float)acc.y / 262144.0;
  // float z_g = (float)acc.z / 262144.0;

  char buf[64];
  // int len = snprintf(buf, sizeof(buf), "%d,%d,%d,%d,%d\n", micros(), acc.x,
  // acc.y, acc.z, mic); int len = snprintf(buf, sizeof(buf), "%d,%d,%d,%d\n",
  // acc.x, acc.y, acc.z, mic);
  int len = snprintf(buf, sizeof(buf), "%d,%d,%d\n", acc.x, acc.y, acc.z);
  // int len = snprintf(buf, sizeof(buf), "%d,%d,%d\n", x_g, y_g, z_g);
  Serial.write(buf, len);
}

// just for the mic/neopixel
void setup1() {
  i2s.setDATA(I2S_DATA);
  i2s.setBCLK(I2S_BCLK);

  i2s.setBitsPerSample(I2S_BIT);
  i2s.setFrequency(sampling_rate);

  if (!i2s.begin()) {
    Serial.println("I2S error");
    while (1)
      ;
  }

  strip.begin();
  uint32_t col = strip.ColorHSV(134 * 256, 255, 255);
  strip.fill(col);
  strip.show();
}

void loop1() {
  // sample I2S mic
  int32_t left, right;

  while (!i2s.available())
    ;
  i2s.read32(&left, &right);

  rp2040.fifo.push(right >> 8);
}
