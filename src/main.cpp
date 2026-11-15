#include "myRobot.h"

#include <ArduinoOTA.h>

WiFiInfo wi;

void setup() {
  Serial.begin(115200);

  if (initFS() == SYS_SUCCESS) {
    if (wi.initWiFi() == SYS_ERROR) {
      Serial.println("Initializing WiFi failed!");
      while (1) {} // wait until code/network fixed
    } else {
      Serial.println("Initializized WiFi!");
      ArduinoOTA.begin();
    }
  }


  initLEDs(127);
  initColorSensor(HIGH);
  initUltrasonicSensor();
  initCollisionDetection();
  initBatteryMeasurement();
  initWebServer();

  initDriving();
  speedL = speedR = 100;
  drive(FORWARD, speedL, speedR);

  initLinefollower();
}


void loop() {
  ArduinoOTA.handle();
  collisonWarning(5);
  collisionDetection();

  measureTSC();
  setLEDColor(colorState);

  loopCallBMS();

  readLfValues();
  linefollower();
}