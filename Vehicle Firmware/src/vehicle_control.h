/**
 * Description:     Header file for vehicle_control.cpp.
 * 
 * Author:          Eddie Kwak
 * Last Modified:   12/10/2025
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
extern const uint8_t MIN_SPEED_PERCENT;
extern const uint8_t MAX_SPEED_PERCENT;
extern const uint8_t TURN_STOP_THRESHOLD_PERCENT;
extern uint8_t speed_percent;
extern bool forward_direction;
// left = -1, straight = 0, right = 1
// the turning directions on the firmware may be 
// flipped depending on how the motors are oriented
extern int turn_direction; 
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
extern long left_duration;
extern float left_distance_cm;
extern long right_duration;
extern float right_distance_cm;

// flag for autonomous mode
extern bool autonomous_mode;

// stops motors
void stop_motors(void);

// updates motor states
void update_motors(void);

#endif