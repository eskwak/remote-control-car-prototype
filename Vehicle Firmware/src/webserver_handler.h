/**
 * Description:     Header file for webserver_handler.cpp.
 *                  Contains a global instance of WebServer.
 * 
 * Author:          Eddie Kwak
 * Last Modified:   12/8/2025
 */

#ifndef WEBSERVER_HANDLER_HPP
#define WEBSERVER_HANDLER_HPP

#include <WebServer.h>

extern WebServer server;

void handle_root(void);

void handle_control(void);

#endif