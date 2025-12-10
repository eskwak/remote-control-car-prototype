/**
 * Description:     Processes HTTP requests coming from web controller.
 * 
 * Author:          Eddie Kwak
 * Last Modified:   12/9/2025
 */

#include "webserver_handler.h"
#include "vehicle_control.h"

void handle_root(void) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "Web vehicle controller running.");
}

void handle_control(void) {
    server.sendHeader("Access-Control-Allow-Origin", "*");

    // speed
    if (server.hasArg("speed")) {
        int speed = server.arg("speed").toInt();
        if (speed < MIN_SPEED_PERCENT) speed = MIN_SPEED_PERCENT;
        if (speed > MAX_SPEED_PERCENT) speed = MAX_SPEED_PERCENT;
        speed_percent = (uint8_t)speed;
    }

    // forward/backward
    if (server.hasArg("forward")) {
        int is_forward = server.arg("forward").toInt();
        if (is_forward == 1) forward_direction = true;
        else forward_direction = false;
    }

    // turning
    if (server.hasArg("turn")) {
        int turn = server.arg("turn").toInt();
        if (turn < -1) turn = -1;
        if (turn > 1) turn = 1;
        turn_direction = turn;
    }

    // accelerating
    if (server.hasArg("accelerating")) {
        int is_accelerating = server.arg("accelerating").toInt();
        if (is_accelerating == 1) accelerate = true;
        else accelerate = false;
    }

    // autonomous mode
    if (server.hasArg("auto")) {
        int is_auto = server.arg("auto").toInt();
        if (is_auto == 1) autonomous_mode = true;
        else autonomous_mode = false;
    }

    server.send(200, "text/plain", "OK");
}