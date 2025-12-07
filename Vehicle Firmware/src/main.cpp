#include "pinout.hpp"
#include "firebase_and_wifi.hpp"
#include "vehicle_control.hpp"

// setup function contains single execution code
void setup(void) {
  Serial.begin(115200);

  // motor pins
  pinMode(LEFT_MOTOR_PIN_1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN_2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN_1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN_2, OUTPUT); 
  pinMode(LEFT_MOTOR_ENABLE_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_ENABLE_PIN, OUTPUT);

  // ultrasonic sensor pins
  pinMode(FRONT_SENSOR_TRIG_PIN, OUTPUT);
  pinMode(FRONT_SENSOR_ECHO_PIN, INPUT);

  // motor driver pins 
  ledcSetup(LEFT_MOTOR_CHANNEL, frequency, resolution);
  ledcSetup(RIGHT_MOTOR_CHANNEL, frequency, resolution);
  ledcAttachPin(LEFT_MOTOR_ENABLE_PIN, LEFT_MOTOR_CHANNEL);
  ledcAttachPin(RIGHT_MOTOR_ENABLE_PIN, RIGHT_MOTOR_CHANNEL);
  ledcWrite(LEFT_MOTOR_CHANNEL, 0);
  ledcWrite(RIGHT_MOTOR_CHANNEL, 0);

  Serial.printf("PWM initialized: %d Hz\n", frequency);
  Serial.printf("Resolution: %d bits\n", resolution);
  delay(200);

  connect_wifi();
  connect_firebase();
  delay(200);

  stop_motors();
}

// loop function contains repeated execution code
void loop(void) {
  static unsigned long last_poll = 0;
  const unsigned long POLL_INTERVAL_MS = 50;

  if (millis() - last_poll >= POLL_INTERVAL_MS) {
    last_poll = millis();
    
    // trigger and read front ultrasonic sensor
    digitalWrite(FRONT_SENSOR_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(FRONT_SENSOR_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(FRONT_SENSOR_TRIG_PIN, LOW);

    front_duration = pulseIn(FRONT_SENSOR_ECHO_PIN, HIGH);
    front_distance_cm = front_duration * sound_speed / 2;

    read_car_state_from_rtdb();
    update_motors();
  }


  // Serial.print("Distance (cm): ");
  // Serial.println(front_distance_cm);
}
