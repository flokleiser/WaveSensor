// CODE BY LUCA SOMM

#include <LSM6DS3.h>
#include <Wire.h>

#include <Adafruit_NeoPixel.h>
#define LED_PIN 10
Adafruit_NeoPixel strip(8, LED_PIN, NEO_GRB + NEO_KHZ800);

LSM6DS3 myIMU(I2C_MODE, 0x6A); // I2C device address 0x6A
float aX, aY, aZ, gX, gY, gZ;
const float accelerationThreshold = 2.5; // threshold of significant in G's
const int numSamples = 119;
int samplesRead = numSamples;

void setup() {

  Serial.begin(9600);

  strip.begin();
  uint32_t col = strip.ColorHSV(134 * 256, 255, 255);
  strip.fill(col);
  strip.show();

  while (!Serial)
    ;
  if (myIMU.begin() != 0) {
    Serial.println("Device error");
  } else {
    Serial.println("aX,aY,aZ,gX,gY,gZ");
  }
}

void loop() {

  while (samplesRead == numSamples) {
    aX = myIMU.readFloatAccelX();
    aY = myIMU.readFloatAccelY();
    aZ = myIMU.readFloatAccelZ();

    float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

    if (aSum >= accelerationThreshold) {
      samplesRead = 0;
      break;
    }
  }

  while (samplesRead < numSamples) {

    samplesRead++;

    Serial.print(myIMU.readFloatAccelX(), 3);
    Serial.print(',');
    Serial.print(myIMU.readFloatAccelY(), 3);
    Serial.print(',');
    Serial.print(myIMU.readFloatAccelZ(), 3);
    Serial.print(',');
    Serial.print(myIMU.readFloatGyroX(), 3);
    Serial.print(',');
    Serial.print(myIMU.readFloatGyroY(), 3);
    Serial.print(',');
    Serial.print(myIMU.readFloatGyroZ(), 3);
    Serial.println();

    if (samplesRead == numSamples) {
      Serial.println();
    }
  }
}
