#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

// These are the values for my treadmill.
// incline %     Y
//     0        40
//     2        59
//     3        75
//     4        90
//     5       102
//     6       112
//     7       124
//     8       135
//     9       145
//    10       158

#define REED_SWITCH 9
#define MAX_RPMS 5000
#define BUFFER_SIZE (MAX_RPMS)
uint64_t timestamps[BUFFER_SIZE];

void tick()
{
  static uint16_t idx = 0;
  noInterrupts();
  timestamps[idx] = millis();
  ++idx;
  idx = idx % BUFFER_SIZE;
  interrupts();
}

uint16_t getRPMs()
{
  uint64_t cutoff = millis() - 3000;
  uint16_t result = 0;
  // can be optimized by going backward from index
  // but not worth here.
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    if (timestamps[i] > cutoff)
    {
      ++result;
    }
  }

  return result;
}


void setup(void)
{
  Serial.begin(115200);
  Wire.begin();
  while (!Serial)
    ;

  // Try to initialize!
  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
    {
      delay(10);
    }
  }

  attachInterrupt(digitalPinToInterrupt(REED_SWITCH), tick, FALLING);
  pinMode(REED_SWITCH, INPUT_PULLDOWN_SENSE);

  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop(void)
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  uint16_t y = a.acceleration.y * (-100);

  Serial.printf("%5d\n", y);

  delay(1000);
}
