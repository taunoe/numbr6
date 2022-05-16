/************************************************
 * File: main.cpp
 * Project: Vapp
 * Github: https://github.com/taunoe/numbr6
 * Last edited: 16.05.2022
 * Copyright 2022 Tauno Erik
 ************************************************/
#include <Arduino.h>

const uint8_t LATCH_PIN = 11;  // PB3; //11
const uint8_t CLOCK_PIN = 12;  // PB4; //12
const uint8_t DATA_PIN = 8;    // PB0; //8

const uint8_t NUM_OF_NUMS = 15;
const uint8_t NUMBERS[NUM_OF_NUMS] = {
  0b00111111,  // 0
  0b00000110,  // 1
  0b01011011,  // 2
  0b01001111,  // 3
  0b01100110,  // 4
  0b01101101,  // 5
  0b01111101,  // 6
  0b00000111,  // 7
  0b01111111,  // 8
  0b01101111,  // 9
  0b01110111,  // A
  0b01111100,  // b
  0b01111001,  // E
  0b01110001,  // F
  0b01110110,  // H
  //0b0gfedcba,  // H
};


// Initialize data array.
// I have nine 7-segment LEDS.
const uint8_t data_size = 9;
uint8_t data[data_size] = {
  0b00000110,  // 1
  0b01011011,  // 2
  0b01001111,  // 3
  0b01100110,  // 4
  0b01101101,  // 5
  0b01111101,  // 6
  0b00000111,  // 7
  0b01111111,  // 8
  0b01101111,  // 9
};


/*
 * Copy-paste on Arduino function:
 https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/wiring_shift.c
 https://github.com/arduino/ArduinoCore-avr/blob/24e6edd475c287cdafee0a4db2eb98927ce3cf58/cores/arduino/wiring_digital.c
 */
void shift_out(uint8_t data_pin, uint8_t clock_pin,
               uint8_t bit_order, uint8_t val) {
  for (uint8_t i = 0; i < 8; i++) {
    if (bit_order == LSBFIRST) {
      digitalWrite(data_pin, val & 1);
      val >>= 1;
    } else {
      digitalWrite(data_pin, (val & 128) != 0);
      val <<= 1;
  }

  //digitalWrite(clock_pin, HIGH);
  PORTB |= (1<<4); // Set to HIGH
  //digitalWrite(clock_pin, LOW);
  PORTB &= ~(1<<4); // Set to LOW
  }
}


/*
Send data to 7-segments
*/
void output_data(uint8_t data[], uint8_t size) {
  for (uint8_t i = 0; i < size; i++) {

    PORTB &= ~(1<<3); // digitalWrite(LATCH_PIN, LOW);
    //shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data[i]);
    shift_out(DATA_PIN, CLOCK_PIN, MSBFIRST, data[i]);
    PORTB |= (1<<3); // digitalWrite(LATCH_PIN, HIGH);
  }
}


void modify_data(uint8_t pos, uint8_t val) {
  data[pos] = val;
}


void setup() {
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
}

void loop() {

//uint8_t num = 0b00000001;

/*
  for (uint8_t i = 0; i < NUM_OF_NUMS; i++) {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, NUMBERS[i]);
    digitalWrite(LATCH_PIN, HIGH);
    delay(500);
  }
*/
  for (size_t i = 0; i < 10; i++) {
    data[0] = NUMBERS[i];
    output_data(data, data_size);
    delay(500);
  }
  
}
