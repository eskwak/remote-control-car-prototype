/**
 * Description:     Header file for wifi_connection.cpp.
 * 
 * Author:          Eddie Kwak
 * Last Modified:   12/9/2025
 */

#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <WiFi.h>

// wifi config
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// connects wifi to personal hotspot bc eduroam 2 factor auth sucks
void connect_wifi(void);

#endif