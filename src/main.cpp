/************************************************
 * File: main.cpp
 * Project: Vapp
 * MCU Board: Arduino Pro Micro (BTE13-010B, www.betemcu.cn)
 * PIR sensor HC-SR501
 * Github: https://github.com/taunoe/numbr6
 * Last edited: 20.05.2022
 * 
 * Copyright 2022 Tauno Erik
 ************************************************/
#include <Arduino.h>

const uint8_t LATCH_PIN = 11;       // PB3; // 11
const uint8_t CLOCK_PIN = 12;       // PB4; // 12
const uint8_t DATA_PIN = 8;         // PB0; // 8
const uint8_t PIR_SENSOR_PIN = 19;  // A6

// I have nine 7-segment LEDS.
const uint8_t NUM_OF_7SEGS = 9;

const uint8_t NUM_OF_NUMS = 15;

// Elements
const uint8_t DP = 0b10000000;
const uint8_t G  = 0b01000000;
const uint8_t F  = 0b00100000;
const uint8_t E  = 0b00010000;
const uint8_t D  = 0b00001000;
const uint8_t C  = 0b00000100;
const uint8_t B  = 0b00000010;
const uint8_t A  = 0b00000001;

const uint8_t ELEMENTS[8] = {A, B, C, D, E, F, G, DP};

// dp-g-f-e-d-c-b-a
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
  //0bdpgfedcba,  // H
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

// Mode1 time to change numbers
// Speed: higer = lower.
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
Shift data to 7-segments.
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


void all_off() {
  for (size_t i = 0; i < NUM_OF_7SEGS; i++) {
    data[i] = 0;
  }
  shift_out_data(data);
}


void all_selected_elements_on(uint8_t element) {
  uint8_t bit = 0;

  switch (element) {
    case DP: bit = 7; break;
    case  G: bit = 6; break;
    case  F: bit = 5; break;
    case  E: bit = 4; break;
    case  D: bit = 3; break;
    case  C: bit = 2; break;
    case  B: bit = 1; break;
    case  A: bit = 0; break;
    default: break;
  }

  for (size_t i = 0; i < NUM_OF_7SEGS; i++) {
    data[i] |= (1<<bit);
  }
  shift_out_data(data);
}

/*
  Mode 1
  Muudab kõiki numbreid kasvas järjekorras.
  Igal numbril on oma kiirus.
*/
void run_mode1() {
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


/*
Mode2
Blink all elements at once.
*/
void run_mode2() {
  for (uint8_t i = 0; i < 8; i++){
    all_off();
    delay(500);
    all_selected_elements_on(ELEMENTS[i]);
    delay(500);
  }
}

/*
Blink dots_one_by_one
Bottom to top.
*/


void setup() {
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(PIR_SENSOR_PIN, INPUT);

  all_off();
  delay(100);
  // Test 7-segments elements
  run_mode2();
  delay(100);
}

void loop() {
  current_time = millis();

  run_mode1();
  //run_mode2();

}
