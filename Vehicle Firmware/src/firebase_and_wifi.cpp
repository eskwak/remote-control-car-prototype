#include "firebase_and_wifi.hpp"

// const char* WIFI_SSID = "...";
// const char* WIFI_PASSWORD = "...";

// const char* API_KEY = "...";
// const char* DATABASE_URL = "...";

FirebaseData firebase_data;
FirebaseAuth firebase_auth;
FirebaseConfig firebase_config;


void connect_wifi(void) {
    Serial.printf("Connecting to wifi: %s\n", WIFI_SSID); 
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        Serial.printf("waiting\n");
    }
    Serial.printf("%s", WiFi.localIP());
}

void connect_firebase(void) {
    Serial.printf("Connecting to firebase rtdb.\n");

    firebase_config.api_key = API_KEY;
    firebase_config.database_url = DATABASE_URL;

    // anon sign up
    if (Firebase.signUp(&firebase_config, &firebase_auth, "", "")) {
        Serial.printf("Anonymous sign up complete.\n");
    }
    else {
        String error_msg = firebase_config.signer.signupError.message.c_str();
        Serial.printf("Anonymous sign up failed: %s\n", error_msg);
    }

    Firebase.begin(&firebase_config, &firebase_auth);
    Firebase.reconnectWiFi(true);
    Serial.printf("Firebase rtdb initialized.\n");
}