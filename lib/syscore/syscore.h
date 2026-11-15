//
// Created by gabri on 12.02.2026.
//

#ifndef ROBOT_WEMA_SYSCORE_H
#define ROBOT_WEMA_SYSCORE_H


#include <Arduino.h>
#include <WiFi.h>

#ifdef USE_LITTLEFS
    #include <LittleFS.h>
#else
    #include <SPIFFS.h>
#endif


#define MAX_WIFI_CONNECT_TRYS 20

#define SYS_SUCCESS 0
#define SYS_ERROR 1


class WiFiInfo {
private:
    String ssid;
    String passwd;
public:
    WiFiInfo() = default;
    int loadPasswd();
    int connectWiFi();
    int initWiFi();
};

int initFS();


#endif //ROBOT_WEMA_SYSCORE_H