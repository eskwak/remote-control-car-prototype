/**
 * Description:     Handles all Motor control logic and driving decisions.
 * 
 * Author:          Eddie Kwak
 * Last Modified:   12/9/2025     
 */

#include "vehicle_control.h"
#include "gpio.h"

// pwm data
int frequency = 3000;
int resolution = 8;
int MIN_DUTY_CYCLE = 80;
int MIN_TURN_DUTY_CYCLE = 25;

// vehicle data
uint8_t speed_percent = 0;
bool forward_direction = true;
int turn_direction = 0;
bool accelerate = false;

// vehicle control 
unsigned long last_control = 0;

// intervals for ESP32 control
const unsigned long CONTROL_INTERVAL_MS = 20;

// approx sound speed for using ultrasonic sensors
float sound_speed = 0.034;

// front ultrasonic sensor
long front_duration;
float front_distance_cm;
const float FRONT_STOP_DISTANCE_CM = 20.0f;

// back ultrasonic sensor
long back_duration;
float back_distance_cm;
const float BACK_STOP_DISTANCE_CM = 20.0f;

void stop_motors(void) {
    ledcWrite(LEFT_MOTOR_CHANNEL, 0);
    ledcWrite(RIGHT_MOTOR_CHANNEL, 0);
    
    digitalWrite(LEFT_MOTOR_PIN_1, LOW);
    digitalWrite(LEFT_MOTOR_PIN_2, LOW);
    digitalWrite(RIGHT_MOTOR_PIN_1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN_2, LOW);
}

void update_motors(void) {
    if (!accelerate) {
        stop_motors();
        return;
    }

    // stop forward motion if front sensor detects obstancle
    if (forward_direction && front_distance_cm > 0 && front_distance_cm <= FRONT_STOP_DISTANCE_CM){
        stop_motors();
        return;
    }

    // stop backward motion if back sensor detects obstacle
    if (!forward_direction && back_distance_cm > 0 && back_distance_cm <= BACK_STOP_DISTANCE_CM) {
        stop_motors();
        return;
    }

    int base_duty_cycle = 80;
    if (speed_percent <= 80) base_duty_cycle = 80;
    else {
        base_duty_cycle = map(speed_percent, 1, 100, MIN_DUTY_CYCLE, 255);
        if (base_duty_cycle < MIN_DUTY_CYCLE) base_duty_cycle = MIN_DUTY_CYCLE;
        if (base_duty_cycle > 255) base_duty_cycle = 255;
    }

    // left/right duty cycles
    int left_duty_cycle = base_duty_cycle;
    int right_duty_cycle = base_duty_cycle;

    //left turn (directions are now flipped due to orientation of components and wiring)
    if (turn_direction == -1) {
        left_duty_cycle = 0;
        right_duty_cycle = base_duty_cycle;
    }
    // right turn
    else if (turn_direction == 1) {
        right_duty_cycle = 0;
        left_duty_cycle = base_duty_cycle;
    }

    if (forward_direction) {
        digitalWrite(LEFT_MOTOR_PIN_1, LOW);
        digitalWrite(LEFT_MOTOR_PIN_2, HIGH);
        digitalWrite(RIGHT_MOTOR_PIN_1, LOW);
        digitalWrite(RIGHT_MOTOR_PIN_2, HIGH);
    }
    else {
        digitalWrite(LEFT_MOTOR_PIN_1, HIGH);
        digitalWrite(LEFT_MOTOR_PIN_2, LOW);
        digitalWrite(RIGHT_MOTOR_PIN_1, HIGH);
        digitalWrite(RIGHT_MOTOR_PIN_2, LOW);
    }
    ledcWrite(LEFT_MOTOR_CHANNEL, left_duty_cycle);
    ledcWrite(RIGHT_MOTOR_CHANNEL, right_duty_cycle);
}