/**
 * Description:     Main file for remote controlled vehicle.
 * 
 * Author:          Eddie Kwak
 * Last Modified:   12/9/2025
 */

#include "gpio.h"
#include "wifi_connection.h"
#include "vehicle_control.h"
#include "webserver_handler.h"

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
  pinMode(BACK_SENSOR_TRIG_PIN, OUTPUT);
  pinMode(BACK_SENSOR_ECHO_PIN, INPUT);

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

  // Start HTTP server
  server.on("/", HTTP_GET, handle_root);
  server.on("/control", HTTP_GET, handle_control);
  server.begin();
  Serial.println("HTTP control server started");

  stop_motors();

}

// loop for sensor reading and motor updates 
void loop(void) {
  // handle incoming HTTP requests
  server.handleClient();

  // loop for sensor readings and motor update
  if (millis() - last_control >= CONTROL_INTERVAL_MS) {
    last_control = millis();

    // front sensor reading
    digitalWrite(FRONT_SENSOR_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(FRONT_SENSOR_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(FRONT_SENSOR_TRIG_PIN, LOW);
    front_duration = pulseIn(FRONT_SENSOR_ECHO_PIN, HIGH, 6000);
    front_distance_cm = front_duration * sound_speed / 2;

    // back sensor reading
    digitalWrite(BACK_SENSOR_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(BACK_SENSOR_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(BACK_SENSOR_TRIG_PIN, LOW);
    back_duration = pulseIn(BACK_SENSOR_ECHO_PIN, HIGH, 6000);
    back_distance_cm = back_duration * sound_speed / 2;

    update_motors();
  }
}