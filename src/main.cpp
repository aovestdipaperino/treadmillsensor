#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/PIN_WIRE_SCL, /* data=*/PIN_WIRE_SDA, /* reset=*/U8X8_PIN_NONE); // OLEDs without Reset of the Display
#define REED_SWITCH D10
#define MAX_RPMS 10000
#define BUFFER_SIZE (MAX_RPMS / 15)
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
  uint64_t cutoff = millis() - (60000 / 15);
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
  // minor optimization. I couldn't resist
  return result;
}

void setup(void)
{
  attachInterrupt(digitalPinToInterrupt(REED_SWITCH), tick, FALLING);
  pinMode(REED_SWITCH, INPUT_PULLDOWN_SENSE);
  u8x8.begin();
  u8x8.setFlipMode(1); // set number from 1 to 3, the screen word will rotary 180
  u8x8.setFont(u8x8_font_inb21_2x4_f);
}

void loop(void)
{

  u8x8.clearDisplay();
  u8x8.setCursor(0, 0);

  u8x8.print(getRPMs());

  delay(500);
}
