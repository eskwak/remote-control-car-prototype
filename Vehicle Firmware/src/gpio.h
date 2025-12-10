/**
 * Description:     Defines all GPIO pin assignments for ESP32 peripherals.
 * 
 * Author:          Eddie Kwak
 * Last Modified:   12/8/2025
 */

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
const uint8_t BACK_SENSOR_ECHO_PIN = 35;
const uint8_t BACK_SENSOR_TRIG_PIN = 12;
const uint8_t LEFT_SENSOR_ECHO_PIN = 34;
const uint8_t LEFT_SENSOR_TRIG_PIN = 13;
const uint8_t RIGHT_SENSOR_ECHO_PIN = 21;
const uint8_t RIGHT_SENSOR_TRIG_PIN = 19;

// buzzer pin (might scrap this)
const uint8_t PASSIVE_BUZZER_PIN = 18;

#endif