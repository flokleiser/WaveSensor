#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <Wire.h>

#define PIN 10

#define NUMPIXELS 8

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
#endif

  pixels.begin();
}

void loop() {
  pixels.clear();

  for (int i = 0; i < NUMPIXELS; i++) {

    pixels.setPixelColor(i, pixels.Color(0, 150, 0));

    pixels.show();

    delay(DELAYVAL);
  }
}
