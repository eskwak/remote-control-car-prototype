#include "vehicle_control.hpp"
#include "firebase_and_wifi.hpp"
#include "pinout.hpp"

int frequency = 3000;
int resolution = 8;
int MIN_DUTY_CYCLE = 50;
int MIN_TURN_DUTY_CYCLE = 25;

uint8_t speed_percent = 0;
bool forward_direction = true;
int turn_direction = 0;
bool accelerate = false;

float sound_speed = 0.034;

// front ultrasonic sensor
long front_duration;
float front_distance_cm;
const float FRONT_STOP_DISTANCE_CM = 20.0f;

void read_car_state_from_rtdb(void) {
    // speed
    String speed_string = "/car/control/speed";
    if (Firebase.RTDB.getInt(&firebase_data, speed_string)) {
        speed_percent = firebase_data.intData();
        if (speed_percent < 40) speed_percent = 40;
        if (speed_percent > 100) speed_percent = 100;
    }
    else Serial.printf("Speed read failed: %s\n", firebase_data.errorReason().c_str());

    // direction
    String direction_string = "/car/control/forward";
    if (Firebase.RTDB.getBool(&firebase_data, direction_string)) {
        forward_direction = firebase_data.boolData();
    }
    else Serial.printf("Direction read failed: %s\n", firebase_data.errorReason().c_str());

    // turning
    String turning_string = "/car/control/turn";
    if (Firebase.RTDB.getInt(&firebase_data, turning_string)) {
        int turn = firebase_data.intData();
        if (turn < -1) turn = -1;
        if (turn > 1) turn = 1;
        turn_direction = turn;
    }
    else Serial.printf("Turn read failed: %s\n", firebase_data.errorReason().c_str());

    // acceleration
    String acceleration_string = "/car/control/accelerating";
    if (Firebase.RTDB.getBool(&firebase_data, acceleration_string)) {
        accelerate = firebase_data.boolData();
    }
    else Serial.printf("Accelerate read failed: %s\n", firebase_data.errorReason().c_str());
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
    if (!accelerate) {
        stop_motors();
        return;
    }

    // stop forward motion if front sensor detects obstancle
    if (forward_direction && front_distance_cm > 0 && front_distance_cm <= FRONT_STOP_DISTANCE_CM){
        stop_motors();
        return;
    }

    int base_duty_cycle = 50;
    if (speed_percent <= 50) base_duty_cycle = 50;
    else {
        base_duty_cycle = map(speed_percent, 1, 100, MIN_DUTY_CYCLE, 255);
        if (base_duty_cycle < MIN_DUTY_CYCLE) base_duty_cycle = MIN_DUTY_CYCLE;
        if (base_duty_cycle > 255) base_duty_cycle = 255;
    }

    // left/right duty cycles
    int left_duty_cycle = base_duty_cycle;
    int right_duty_cycle = base_duty_cycle;

    // left turn
    if (turn_direction == -1) {
        right_duty_cycle = base_duty_cycle;
        left_duty_cycle = base_duty_cycle / 2;
        if (left_duty_cycle > 0 && left_duty_cycle < MIN_TURN_DUTY_CYCLE) {
            left_duty_cycle = MIN_TURN_DUTY_CYCLE;
        }
    }
    // right turn
    else if (turn_direction == 1) {
        left_duty_cycle = base_duty_cycle;
        right_duty_cycle = base_duty_cycle / 2;
        if (right_duty_cycle > 0 && right_duty_cycle < MIN_TURN_DUTY_CYCLE) {
            right_duty_cycle = MIN_TURN_DUTY_CYCLE;
        }
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