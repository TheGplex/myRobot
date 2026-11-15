//
// Created by gabri on 12.02.2026.
//

#include "syscore.h"


IPAddress local_IP(192, 168, 1, 20);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// IPAddress local_IP(10, 115, 61, 20);
// IPAddress gateway(10, 115, 0, 1);
// IPAddress subnet(255, 255, 192, 0);

IPAddress primaryDNS(1, 1, 1, 1);
IPAddress secondaryDNS(1, 0, 0, 1);


int WiFiInfo::loadPasswd() {
    #ifdef USE_LITTLEFS
        File wifiFile = LittleFS.open("/WiFi.txt", FILE_READ);
    #else
        File wifiFile = SPIFFS.open("/WiFi.txt", FILE_READ);
    #endif

    if (!wifiFile) {
        Serial.println("Couldn't read file!");
        return SYS_ERROR;
    }

    passwd = wifiFile.readStringUntil('\n');
    passwd.trim();
    ssid = wifiFile.readStringUntil('\n');
    ssid.trim();

    wifiFile.close();
    return SYS_SUCCESS;
}

int WiFiInfo::connectWiFi() {
    uint8_t connect_trys = 0;
    WiFi.mode(WIFI_STA);

    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("STA Failed to configure");
        return SYS_ERROR;
    }

    WiFi.begin(ssid, passwd);

    Serial.print("Connecting to WiFI");

    while (WiFi.status() != WL_CONNECTED && connect_trys <= MAX_WIFI_CONNECT_TRYS) {
        Serial.print(".");
        connect_trys++;
        delay(500);
    }

    Serial.println("\nLocal IP: " + WiFi.localIP().toString());

    return connect_trys > MAX_WIFI_CONNECT_TRYS;
}

int WiFiInfo::initWiFi() {
    if(loadPasswd() == SYS_ERROR) {
        Serial.println("Couldn't load password or SSID");
        return SYS_ERROR;
    }
    if(connectWiFi() == SYS_ERROR) {
        Serial.println("Couldn't connect to WiFi");
        return SYS_ERROR;
    }

    return SYS_SUCCESS;
}


int initFS() {
    #ifdef USE_LITTLEFS
        if (!LittleFS.begin()) {
            Serial.println("LittleFS failed");
            return SYS_ERROR;
        }
    #else
        if (!SPIFFS.begin()) {
            Serial.println("SPIFFS failed");
            return SYS_ERROR;
        }
    #endif

    Serial.println("Filesystem successfully initialized");

    return SYS_SUCCESS;
}