#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// wifi config
const char* WIFI_SSID = "Eddie";
const char* WIFI_PASSWORD = "Eddie123";

// firebase config
const char* API_KEY = "AIzaSyBGXfmSrV504OtG8232OJ1NKeNPC2y6s3o";
const char* DATABASE_URL = "https://autonomous-vehicle-985f1-default-rtdb.firebaseio.com";

// firebase objects
FirebaseData firebase_data;
FirebaseAuth firebase_auth;
FirebaseConfig firebase_config;

// left motor config
const uint8_t LEFT_MOTOR_CHANNEL = 0;
const uint8_t LEFT_MOTOR_PIN_1 = 27;
const uint8_t LEFT_MOTOR_PIN_2 = 26;
const uint8_t LEFT_MOTOR_ENABLE_PIN = 14;

// right motor config
const uint8_t RIGHT_MOTOR_CHANNEL = 1;
const uint8_t RIGHT_MOTOR_PIN_1 = 25;
const uint8_t RIGHT_MOTOR_PIN_2 = 33;
const uint8_t RIGHT_MOTOR_ENABLE_PIN = 32;

// PWM config - L298N works best with 1-5kHz frequency
// Lower frequencies (1-3kHz) are more audible but L298N responds best
// 3kHz is a good compromise - some noise but reliable operation
const int frequency = 3000;  // 3kHz - reliable for L298N, some audible noise
const int resolution = 8;
const int MIN_DUTY_CYCLE = 60;  // Minimum duty cycle to overcome motor static friction (0-255)
                                 // Lowered - motors seem to work better with less aggressive minimum

// vehicle states
uint8_t speed_percent = 0;
bool forward_direction = true;
int turn_direction = 0; // left = -1 | 0 = straight | 1 = right
bool accelerate = false;

// function prototypes
void connect_wifi(void);
void connect_firebase(void);
void read_car_state_from_rtdb(void);
void update_motors(void);
void stop_motors(void);

// setup function contains single execution code
void setup(void) {
  Serial.begin(115200);

  // motor pins
  pinMode(LEFT_MOTOR_PIN_1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN_2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN_1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN_2, OUTPUT);
  
  // Enable pins should be set as OUTPUT (ledcAttachPin handles this, but being explicit)
  pinMode(LEFT_MOTOR_ENABLE_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_ENABLE_PIN, OUTPUT);

  // PWM setup for L298N motor driver
  ledcSetup(LEFT_MOTOR_CHANNEL, frequency, resolution);
  ledcSetup(RIGHT_MOTOR_CHANNEL, frequency, resolution);
  ledcAttachPin(LEFT_MOTOR_ENABLE_PIN, LEFT_MOTOR_CHANNEL);
  ledcAttachPin(RIGHT_MOTOR_ENABLE_PIN, RIGHT_MOTOR_CHANNEL);
  ledcWrite(LEFT_MOTOR_CHANNEL, 0);
  ledcWrite(RIGHT_MOTOR_CHANNEL, 0);
  
  Serial.printf("PWM initialized: %d Hz, %d-bit resolution\n", frequency, resolution);
  Serial.printf("L298N LED is ON - power confirmed.\n");
  Serial.printf("\n*** IMPORTANT: If motors beep but don't spin ***\n");
  Serial.printf("1. CHECK L298N ENABLE JUMPERS - They MUST be REMOVED!\n");
  Serial.printf("   (Look for small jumpers on ENA/ENB pins - remove them!)\n");
  Serial.printf("2. Verify enable pins: Left=GPIO%d, Right=GPIO%d\n", 
                LEFT_MOTOR_ENABLE_PIN, RIGHT_MOTOR_ENABLE_PIN);
  Serial.printf("3. Check motor wiring to L298N output terminals\n");
  Serial.printf("4. Minimum duty cycle set to %d/255 (increase if needed)\n", MIN_DUTY_CYCLE);
  Serial.printf("\n");

  connect_wifi();
  connect_firebase();

  stop_motors();
}

// loop function contains repeated execution code
void loop(void) {
  static unsigned long last_poll = 0;
  const unsigned long POLL_INTERVAL_MS = 50;

  if (millis() - last_poll >= POLL_INTERVAL_MS) {
    last_poll = millis();
    read_car_state_from_rtdb();
    update_motors();
  }
}

// connects wifi to personal hotspot bc eduroam 2 factor auth sucks
void connect_wifi(void) {
  Serial.printf("Connecting to wifi: %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.printf(".");
  }

  Serial.printf("\n");
  Serial.printf("Wifi connected.\n");
  Serial.printf("IP address: %s\n", WiFi.localIP());
}

// connects to firebase 
void connect_firebase(void) {
  Serial.printf("Configuring firebase rtdb...\n");

  firebase_config.api_key = API_KEY;
  firebase_config.database_url = DATABASE_URL;

  // Try anonymous sign-up. Requires "Anonymous" provider enabled in Firebase console.
  if (Firebase.signUp(&firebase_config, &firebase_auth, "", "")) {
    Serial.printf("Firebase anonymous sign up completed.\n");
  }
  else {
    Serial.printf("Firebase anonymous sign up failed: %s\n",
                  firebase_config.signer.signupError.message.c_str());
  }

  Firebase.begin(&firebase_config, &firebase_auth);
  Firebase.reconnectWiFi(true);
  Serial.printf("Firebase initialized.\n");
}

// reads car state from firebase rtdb
void read_car_state_from_rtdb(void) {
  // speed
  String speed_string = "/car/control/speed";
  if (Firebase.RTDB.getInt(&firebase_data, speed_string)) {
    speed_percent = firebase_data.intData();
    if (speed_percent < 0) speed_percent = 0;
    if (speed_percent > 100) speed_percent = 100;
  } else {
    Serial.printf("Speed read failed: %s\n", firebase_data.errorReason().c_str());
  }

  // direction
  String direction_string = "/car/control/forward";
  if (Firebase.RTDB.getBool(&firebase_data, direction_string)) {
    forward_direction = firebase_data.boolData();
  } else {
    Serial.printf("Direction read failed: %s\n", firebase_data.errorReason().c_str());
  }

  // turning
  String turning_string = "/car/control/turn";
  if (Firebase.RTDB.getInt(&firebase_data, turning_string)) {
    int turn = firebase_data.intData();
    if (turn < -1) turn = -1;
    if (turn > 1) turn = 1;
    turn_direction = turn;
  } else {
    Serial.printf("Turn read failed: %s\n", firebase_data.errorReason().c_str());
  }

  // acceleration
  String acceleration_string = "/car/control/accelerating";
  if (Firebase.RTDB.getBool(&firebase_data, acceleration_string)) {
    accelerate = firebase_data.boolData();
  } else {
    Serial.printf("Accelerate read failed: %s\n", firebase_data.errorReason().c_str());
  }

  // printing control data to serial monitor for debug
  Serial.printf("speed = %d, forward = %d, turn = %d, accel = %d\n", speed_percent, (int)forward_direction, turn_direction, (int)accelerate);
}

// stops motors
void stop_motors(void) {
  ledcWrite(LEFT_MOTOR_CHANNEL, 0);
  ledcWrite(RIGHT_MOTOR_CHANNEL, 0);

  digitalWrite(LEFT_MOTOR_PIN_1, LOW);
  digitalWrite(LEFT_MOTOR_PIN_2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN_1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN_2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN_2, LOW);
}

// updates motor control
void update_motors(void) {
  // vehicle does not move if speed is 0
  if (speed_percent <= 0) {
    stop_motors();
    return;
  }
  
  // Safety check: if accelerate is false, stop motors (emergency brake)
  if (!accelerate) {
    stop_motors();
    return;
  }

  // mapping 0-100% speed to 0-255 duty cycle
  // Use a better mapping that works at lower speeds
  // Map 0-100% to MIN_DUTY_CYCLE-255 for better low-speed control
  int base_duty_cycle;
  if (speed_percent <= 0) {
    base_duty_cycle = 0;
  } else {
    // Map speed to duty cycle range, ensuring minimum for motor start
    base_duty_cycle = map(speed_percent, 1, 100, MIN_DUTY_CYCLE, 255);
    if (base_duty_cycle < MIN_DUTY_CYCLE) base_duty_cycle = MIN_DUTY_CYCLE;
    if (base_duty_cycle > 255) base_duty_cycle = 255;
  }

  // left/right duty cycles
  int left_duty_cycle = base_duty_cycle;
  int right_duty_cycle = base_duty_cycle;

  // left turn
  if (turn_direction == -1) {
    left_duty_cycle = base_duty_cycle / 2;
    // Ensure minimum duty cycle even when turning
    if (left_duty_cycle > 0 && left_duty_cycle < MIN_DUTY_CYCLE) {
      left_duty_cycle = MIN_DUTY_CYCLE;
    }
    right_duty_cycle = base_duty_cycle;
  }
  // right turn
  else if (turn_direction == 1) {
    left_duty_cycle = base_duty_cycle;
    right_duty_cycle = base_duty_cycle / 2;
    // Ensure minimum duty cycle even when turning
    if (right_duty_cycle > 0 && right_duty_cycle < MIN_DUTY_CYCLE) {
      right_duty_cycle = MIN_DUTY_CYCLE;
    }
  }

  // direction handling
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
  
  // Write PWM values to enable pins
  ledcWrite(LEFT_MOTOR_CHANNEL, left_duty_cycle);
  ledcWrite(RIGHT_MOTOR_CHANNEL, right_duty_cycle);
  
  // DEBUGGING: Uncomment below to test with FULL POWER (bypasses speed control)
  // This helps determine if it's a PWM/speed issue or wiring issue
  // ledcWrite(LEFT_MOTOR_CHANNEL, 255);
  // ledcWrite(RIGHT_MOTOR_CHANNEL, 255);
  // Serial.printf("*** TEST MODE: Full power enabled ***\n");
  
  // Detailed debug output with diagnostic info
  int l1 = digitalRead(LEFT_MOTOR_PIN_1);
  int l2 = digitalRead(LEFT_MOTOR_PIN_2);
  int r1 = digitalRead(RIGHT_MOTOR_PIN_1);
  int r2 = digitalRead(RIGHT_MOTOR_PIN_2);
  
  // Check if motors are in brake mode (both direction pins same)
  String left_status = (l1 == l2) ? "BRAKE!" : "OK";
  String right_status = (r1 == r2) ? "BRAKE!" : "OK";
  
  // Read enable pin states (if not in test mode, these are PWM so reading may not be accurate)
  int left_enable_state = digitalRead(LEFT_MOTOR_ENABLE_PIN);
  int right_enable_state = digitalRead(RIGHT_MOTOR_ENABLE_PIN);
  
  Serial.printf("Motors: L_PWM=%d, R_PWM=%d | L1=%d, L2=%d [%s] | R1=%d, R2=%d [%s] | ENA_L=%d, ENA_R=%d | dir=%s\n", 
                left_duty_cycle, right_duty_cycle,
                l1, l2, left_status.c_str(),
                r1, r2, right_status.c_str(),
                left_enable_state, right_enable_state,
                forward_direction ? "forward" : "reverse");
}