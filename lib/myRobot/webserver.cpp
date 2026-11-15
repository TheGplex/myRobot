#include "myRobot.h"
#include "syscore.h"

AsyncWebServer server(80);


void initWebServer() {
    server.on("/", HTTP_GET, [=](AsyncWebServerRequest *req) {
        #ifdef USE_LITTLEFS
            req->send(LittleFS, "/index.html", "text/html", false, [=](const String& str) -> String {
                if (str == "BAT_PCT") {
                    return getBatteryPercentageString();
                } else if (str == "BAT_LVL") {
                    return getBatteryLevelString();
                } else if (str == "BAT_STAT") {
                    return getBatteryStateString();
                } else if (str == "LF_VAL") {
                    return getLfValuesString();
                }
                return String();
            });
        #else
            req->send(SPIFFS, "/index.html", "text/html", false, [=](const String& str) -> String {
                if (str == "BATTERY_PERCENTAGE") {
                    return getBatteryPercentageString();
                } else if (str == "BATTERY_LEVEL") {
                    return getBatteryLevelString();
                } else if (str == "BATTERY_STATE") {
                    return getBatteryStateString();
                }
                return String();
            });
        #endif
    });

    server.on("/refreshBatteryPercent", HTTP_GET, [=](AsyncWebServerRequest *req) {
        req->send(200, "text/plain", getBatteryPercentageString());
    });

    server.on("/refreshBatteryState", HTTP_GET, [=](AsyncWebServerRequest *req) {
        req->send(200, "text/plain", getBatteryStateString());
    });

    server.on("/refreshBatteryLevel", HTTP_GET, [=](AsyncWebServerRequest *req) {
        req->send(200, "text/plain", getBatteryLevelString());
    });

    server.on("/refreshLfValues", HTTP_GET, [=](AsyncWebServerRequest *req) {
        req->send(200, "text/plain", getLfValuesString());
    });


    #ifdef USE_LITTLEFS
        server.serveStatic("/", LittleFS, "/");
    #else
        server.serveStatic("/", SPIFFS, "/");
    #endif

    server.begin();

    Serial.println("Webserver started");
}


String getBatteryPercentageString() {
    String batteryPercentageStr = "";
    const int pct = calcBattery_Percentage_State();

    batteryPercentageStr += String(pct) + " &#37"; // normal % due to template handler not possible, &#37 = %
    return batteryPercentageStr;
}


String getBatteryStateString() {
    String batteryStateStr = "";

    switch(battery_state) {
        case FULL_S: batteryStateStr     = "Full"; break;
        case MEDIUM_S: batteryStateStr   = "Medium"; break;
        case LOW_S: batteryStateStr      = "Low"; break;
        case EMPTEY_S: batteryStateStr   = "Emptey"; break;
        default: batteryStateStr         = "Unknown"; break;

    }

    return batteryStateStr;
}


String getBatteryLevelString() {
    String batteryLevelStr = String(currentVoltage, 2) + " V";  // 2 decimal places
    return batteryLevelStr;
}


String getLfValuesString() {
    String lfValuesStr = "";

    #ifdef USE_LINEFOLLOWER
        lfValuesStr = "L: " + String(lfValueAvg[LF_VALUE_LEFT]) + " | R: " + String(lfValueAvg[LF_VALUE_RIGHT]);
    #else
        lfValuesStr = "Linefollower not initialized!";
    #endif
    
    return lfValuesStr;
}