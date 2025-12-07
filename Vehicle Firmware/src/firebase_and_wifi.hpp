#ifndef FIREBASE_AND_WIFI_HPP
#define FIREBASE_AND_WIFI_HPP

#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// wifi config
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// firebase config
extern const char* API_KEY;
extern const char* DATABASE_URL;

// firebase objects
extern FirebaseData firebase_data;
extern FirebaseAuth firebase_auth;
extern FirebaseConfig firebase_config;

// connects wifi to personal hotspot bc eduroam 2 factor auth sucks
void connect_wifi(void);

// connects to firebase using anon sign up
void connect_firebase(void);


#endif