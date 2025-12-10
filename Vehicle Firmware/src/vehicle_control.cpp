/**
 * Description:     Handles all Motor control logic and driving decisions.
 *                  The 'autonomous' mode is very limited. It works under the condition that 
 *                  there are tall enough walls to be consistently be detected by the four 
 *                  ultrasonic sensors
 * 
 * Author:          Eddie Kwak
 * Last Modified:   12/10/2025     
 */

#include "vehicle_control.h"
#include "gpio.h"
#include <cmath>

// pwm data
int frequency = 15000;
int resolution = 8;
int MIN_DUTY_CYCLE = 5;
int MIN_TURN_DUTY_CYCLE = 0;
const float TURN_SPEED_FACTOR = 0.65f;

// vehicle data
const uint8_t MIN_SPEED_PERCENT = 5;
const uint8_t MAX_SPEED_PERCENT = 80;
const uint8_t TURN_STOP_THRESHOLD_PERCENT = 20;
uint8_t speed_percent = 0;
bool forward_direction = true;
int turn_direction = 0;
bool accelerate = false;

// vehicle control 
unsigned long last_control = 0;

// intervals for ESP32 control
const unsigned long CONTROL_INTERVAL_MS = 10;

// approx sound speed for using ultrasonic sensors
float sound_speed = 0.034;

// front ultrasonic sensor
long front_duration;
float front_distance_cm;
const float FRONT_STOP_DISTANCE_CM = 25.0f;

// back ultrasonic sensor
long back_duration;
float back_distance_cm;
const float BACK_STOP_DISTANCE_CM = 25.0f;

// left ultrasonic sensor
long left_duration;
float left_distance_cm;

// right ultrasonic sensor 
long right_duration;
float right_distance_cm;

// autonomous mode flag
bool autonomous_mode = false;

// converts a percentage speed input to a PWM duty cycle while respecting limits
static int compute_duty_from_percent(uint8_t percent) {
    int clamped_percent = percent;
    if (clamped_percent < MIN_SPEED_PERCENT) clamped_percent = MIN_SPEED_PERCENT;
    if (clamped_percent > MAX_SPEED_PERCENT) clamped_percent = MAX_SPEED_PERCENT;

    int duty_cycle = map(clamped_percent, MIN_SPEED_PERCENT, 100, MIN_DUTY_CYCLE, 255);
    if (duty_cycle < MIN_DUTY_CYCLE) duty_cycle = MIN_DUTY_CYCLE;
    if (duty_cycle > 255) duty_cycle = 255;
    return duty_cycle;
}

// returns duty cycle for inner wheel while turning; can force aggressive stop
static int compute_inner_duty(int base_duty_cycle, uint8_t current_speed_percent, bool aggressive_turn) {
    if (aggressive_turn) return MIN_TURN_DUTY_CYCLE;
    if (current_speed_percent < TURN_STOP_THRESHOLD_PERCENT) return MIN_TURN_DUTY_CYCLE;

    int duty_cycle = static_cast<int>(base_duty_cycle * TURN_SPEED_FACTOR);
    if (duty_cycle < MIN_TURN_DUTY_CYCLE) duty_cycle = MIN_TURN_DUTY_CYCLE;
    return duty_cycle;
}

void stop_motors(void) {
    ledcWrite(LEFT_MOTOR_CHANNEL, 0);
    ledcWrite(RIGHT_MOTOR_CHANNEL, 0);
    
    digitalWrite(LEFT_MOTOR_PIN_1, LOW);
    digitalWrite(LEFT_MOTOR_PIN_2, LOW);
    digitalWrite(RIGHT_MOTOR_PIN_1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN_2, LOW);
}

void update_motors(void) {
    // start of autonomous mode //
    if (autonomous_mode) {
        forward_direction = true;

        // speed is set low because sensors can't keep up with faster movement
        const uint8_t AUTO_SPEED_PERCENT = 35;
        const float LEFT_RIGHT_DIFFERENCE_THRESHOLD = 5.0f;
        const bool AGGRESSIVE_TURN = true;
        const bool SOFT_TURN = false;

        int base_duty_cycle = compute_duty_from_percent(AUTO_SPEED_PERCENT);
        int left_duty_cycle = base_duty_cycle;
        int right_duty_cycle = base_duty_cycle;

        bool front_blocked = front_distance_cm > 0 && front_distance_cm <= FRONT_STOP_DISTANCE_CM;
        bool has_left = left_distance_cm > 0;
        bool has_right = right_distance_cm > 0;

        float left_for_compare = has_left ? left_distance_cm : 10000.0f;
        float right_for_compare = has_right ? right_distance_cm : 10000.0f;

        if (front_blocked) {
            // if left side has more free space, turn left
            // if not, turn right
            // if indecisive, turn right and poll the ultrasonic sensors again
            if (left_for_compare > right_for_compare) turn_direction = -1;
            else if (right_for_compare > left_for_compare) turn_direction = 1;
            else turn_direction = 1;

            if (turn_direction == -1) {
                left_duty_cycle = compute_inner_duty(base_duty_cycle, AUTO_SPEED_PERCENT, AGGRESSIVE_TURN);
                right_duty_cycle = base_duty_cycle;
            }
            else {
                right_duty_cycle = compute_inner_duty(base_duty_cycle, AUTO_SPEED_PERCENT, AGGRESSIVE_TURN);
                left_duty_cycle = base_duty_cycle;
            }
        }
        else {
            if (has_left && has_right) {
                float delta = left_distance_cm - right_distance_cm;
                // if the absolute value of the left & right distances is greater than the threshold, turn
                if (fabsf(delta) > LEFT_RIGHT_DIFFERENCE_THRESHOLD) {
                    if (delta > 0) turn_direction = -1;
                    else turn_direction = 1;
                } else {
                    turn_direction = 0;
                }
            }
            // turn right if there's an object to the left and free space to the right
            else if (has_left && !has_right) {
                turn_direction = 1;
            }
            // turn left if there's an object to the right and free space to the left
            else if (!has_left && has_right) {
                turn_direction = -1;
            }
            // no objects detected within range -> just go straight
            else {
                turn_direction = 0;
            }

            if (turn_direction == -1) {
                left_duty_cycle = compute_inner_duty(base_duty_cycle, AUTO_SPEED_PERCENT, SOFT_TURN);
                right_duty_cycle = base_duty_cycle;
            }
            else if (turn_direction == 1) {
                right_duty_cycle = compute_inner_duty(base_duty_cycle, AUTO_SPEED_PERCENT, SOFT_TURN);
                left_duty_cycle = base_duty_cycle;
            }
        }

        digitalWrite(LEFT_MOTOR_PIN_1, LOW);
        digitalWrite(LEFT_MOTOR_PIN_2, HIGH);
        digitalWrite(RIGHT_MOTOR_PIN_1, LOW);
        digitalWrite(RIGHT_MOTOR_PIN_2, HIGH);

        ledcWrite(LEFT_MOTOR_CHANNEL, left_duty_cycle);
        ledcWrite(RIGHT_MOTOR_CHANNEL, right_duty_cycle);
        
        return;
    }
    // end of autonomous mode //

    // manual mode
    if (!accelerate) {
        stop_motors();
        return;
    }

    // stop forward movement if front sensor detects something
    if (forward_direction && front_distance_cm > 0 && front_distance_cm <= FRONT_STOP_DISTANCE_CM){
        stop_motors();
        return;
    }

    // stop backwards movement if back sensor detects something
    if (!forward_direction && back_distance_cm > 0 && back_distance_cm <= BACK_STOP_DISTANCE_CM) {
        stop_motors();
        return;
    }

    // take speed percent and calculate base duty cycle (duty cycle without turning)
    int base_duty_cycle = compute_duty_from_percent(speed_percent);

    // default straight
    int left_duty_cycle = base_duty_cycle;
    int right_duty_cycle = base_duty_cycle;

    if (turn_direction == -1) {
        left_duty_cycle = compute_inner_duty(base_duty_cycle, speed_percent, false);
        right_duty_cycle = base_duty_cycle;
    }
    else if (turn_direction == 1) {
        right_duty_cycle = compute_inner_duty(base_duty_cycle, speed_percent, false);
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