/************************************************
 * File: main.cpp
 * Project: Vapp
 * Github: https://github.com/taunoe/numbr6
 * Last edited: 18.05.2022
 * Copyright 2022 Tauno Erik
 ************************************************/
#include <Arduino.h>

const uint8_t LATCH_PIN = 11;  // PB3; // 11
const uint8_t CLOCK_PIN = 12;  // PB4; // 12
const uint8_t DATA_PIN = 8;    // PB0; // 8

// I have nine 7-segment LEDS.
const uint8_t NUM_OF_7SEGS = 9;

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
uint8_t data[NUM_OF_7SEGS] = {
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

// Timing arrays
uint32_t current_time = 0;

uint32_t prev_time[NUM_OF_7SEGS] = {0,0,0,0,0,0,0,0,0};

bool update[NUM_OF_7SEGS] = {
  false,  // 0
  false,  // 1
  false,  // 2
  false,  // 3
  false,  // 4
  false,  // 5
  false,  // 6
  false,  // 7
  false   // 8
};

const uint32_t TIME[NUM_OF_7SEGS] = {
  500,  // 0
  600,  // 1
  700,  // 2
  800,  // 3
  900,  // 4
  1000, // 5
  1100, // 6
  1200, // 7
  1300  // 8
};

uint8_t count[NUM_OF_7SEGS] = {0,0,0,0,0,0,0,0,0};

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


void shift_out_new(uint8_t val) {
  for (uint8_t i = 0; i < 8; i++) {
    
    uint8_t data = (val & 128) != 0;
    
    if (data == 0) {
      PORTB &= ~(1);
    } else {
      PORTB |= 1;
    }
    val <<= 1;

  PORTB |= (1<<4); // Set to HIGH
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
    shift_out_new(data[i]);
    PORTB |= (1<<3); // digitalWrite(LATCH_PIN, HIGH);
  }
}


/*
Shift data to 7-segments
*/
void shift_out_data(uint8_t data[]) {
  for (uint8_t segment = 0; segment < NUM_OF_7SEGS; segment++) {
    PORTB &= ~(1<<3);  // digitalWrite(LATCH_PIN, LOW);
    //shift_out_new(DATA_PIN, CLOCK_PIN, data[i]);
    //
    uint8_t d = data[segment];
    for (uint8_t i = 0; i < 8; i++) {
      uint8_t bit = (d & 128) != 0;
      if (bit == 0) {
        PORTB &= ~(1);
      } else {
        PORTB |= 1;
      }
      d <<= 1;

      PORTB |= (1<<4);   // Set to HIGH
      PORTB &= ~(1<<4);  // Set to LOW
    }
    //
    PORTB |= (1<<3);  // digitalWrite(LATCH_PIN, HIGH);
  }
}

/*
*/
void mode1() {
  // Timing
  for (size_t i = 0; i < NUM_OF_7SEGS; i++) {
    if (current_time - prev_time[i] >= TIME[i]) {
      update[i] = true;
      prev_time[i] = current_time; 
    }
  }

  // Update 9 7-SEGMENTS
  for (size_t i = 0; i < NUM_OF_7SEGS; i++) {
    if (update[i]) {
      data[i] = NUMBERS[count[i]];
      //output_data(data, NUM_OF_7SEGS);
      shift_out_data(data);
      count[i]++;
      if (count[i] > 9) {
        count[i] = 0;
      }
      update[i] = false;
    }
  }
}


void setup() {
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
}

void loop() {
  current_time = millis();

  mode1();

  /******
  Mode 1
  *******/
 /*
  for (size_t i = 0; i < NUM_OF_7SEGS; i++) {
    if (current_time - prev_time[i] >= TIME[i]) {
      update[i] = true;
      prev_time[i] = current_time; 
    }
  }

  // Update 9 7-SEGMENTS
  for (size_t i = 0; i < NUM_OF_7SEGS; i++) {
    if (update[i]) {
      data[i] = NUMBERS[count[i]];
      //output_data(data, NUM_OF_7SEGS);
      shift_out_data(data);
      count[i]++;
      if (count[i] > 9) {
        count[i] = 0;
      }
      update[i] = false;
    }
  }
  */

}
