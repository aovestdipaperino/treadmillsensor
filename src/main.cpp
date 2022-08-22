#include <Arduino.h>
#include <SPI.h>
#include "OSO_LCD.h"


#define REED_SWITCH 10
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
OSO_LCDWing display;
char s_buffer[10];
void setup(void)
{
  display.begin();
  attachInterrupt(digitalPinToInterrupt(REED_SWITCH), tick, FALLING);
  pinMode(REED_SWITCH, INPUT_PULLDOWN_SENSE);
}

void loop(void)
{
  for(int i=0;i < 10;i++) s_buffer[i] = ' ';
  sprintf(s_buffer, "%5d", getRPMs());
  display.print(s_buffer);

 delay(500);
}
