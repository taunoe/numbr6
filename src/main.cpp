#include <Arduino.h>

const uint8_t LATCH_PIN = 8;
const uint8_t CLOCK_PIN = 12;
const uint8_t DATA_PIN = 11;

void setup() {
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
}

void loop() {
  // count from 0 to 255 and display the number

// on the LEDs

for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) {

// take the latchPin low so

// the LEDs don't change while you're sending in bits:

digitalWrite(LATCH_PIN, LOW);

// shift out the bits:

shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, numberToDisplay);

//take the latch pin high so the LEDs will light up:

digitalWrite(LATCH_PIN, HIGH);

// pause before next value:

delay(500);

}
}