/**
 * Description:     Implementing wifi connection.
 *                  Contains wifi connection credentials.
 * 
 * Author:          Eddie Kwak
 * Last Modified:   12/9/2025
 */

#include "wifi_connection.h"
#include "vehicle_control.h"
#include <WebServer.h>

const char* WIFI_SSID = "esk-xps";
const char* WIFI_PASSWORD = "esk-xps-password";

// const char* WIFI_SSID = "...";
// const char* WIFI_PASSWORD = "...";

WebServer server(80);

void connect_wifi(void) {
    Serial.printf("Connecting to wifi: %s\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        Serial.printf("waiting\n");
    }
    Serial.printf("Connected. IP address: %s\n", WiFi.localIP().toString().c_str());
}