/**
 * Description:     Header file for vehicle_control.cpp.
 * 
 * Author:          Eddie Kwak
 * Last Modified:   12/9/2025
 */

#ifndef VEHICLE_CONTROL_HPP
#define VEHICLE_CONTROL_HPP

#include <cstdint>

// PWM config
extern int frequency;
extern int resolution;
extern int MIN_DUTY_CYCLE;
extern int MIN_TURN_DUTY_CYCLE;

// vehicle states
extern uint8_t speed_percent;
extern bool forward_direction;
extern int turn_direction; // left = -1, straight = 0, right = 1
extern bool accelerate;

// control variables
extern unsigned long last_control;

// intervals for updating controls
extern const unsigned long CONTROL_INTERVAL_MS;

// sound speed for ultrasonic sensor
extern float sound_speed;

// front ultrasonic sensor variables
extern long front_duration;
extern float front_distance_cm;
extern long back_duration;
extern float back_distance_cm;

// stops motors
void stop_motors(void);

// updates motor states
void update_motors(void);

#endif