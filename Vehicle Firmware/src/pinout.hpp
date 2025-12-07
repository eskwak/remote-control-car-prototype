#ifndef PINOUT_HPP
#define PINOUT_HPP

#include <Arduino.h>

// motor pins
const uint8_t LEFT_MOTOR_CHANNEL = 0;
const uint8_t RIGHT_MOTOR_CHANNEL = 1;
const uint8_t LEFT_MOTOR_PIN_1 = 27;
const uint8_t LEFT_MOTOR_PIN_2 = 26;
const uint8_t RIGHT_MOTOR_PIN_1 = 25;
const uint8_t RIGHT_MOTOR_PIN_2 = 33;
const uint8_t LEFT_MOTOR_ENABLE_PIN = 14;
const uint8_t RIGHT_MOTOR_ENABLE_PIN = 32;

// ultrasonic sensor pins
const uint8_t FRONT_SENSOR_ECHO_PIN = 23; 
const uint8_t FRONT_SENSOR_TRIG_PIN = 22;

#endif