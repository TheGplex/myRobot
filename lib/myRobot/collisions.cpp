#include "myRobot.h"

void collisonWarning(int distanceLimit) {
    static bool blinkingBright = false;
    static uint64_t lastDistanceMeasureTime = 0;
    static uint64_t lastBlinkTime = 0;

    if (distanceLimit < US_MIN_RANGE || distanceLimit > US_MAX_RANGE) {
        Serial.println("PLEASE CHOOSE A distanceLimit TO BE BETWEEN 3 CM - 400 CM!");
        return;
    }

    if (millis() - lastDistanceMeasureTime > 250) {
        float distance = getDistanceCM();
        // Serial.println(distance);
        if ((distance <= distanceLimit && distance != US_TOO_FAR) || distance == US_TOO_CLOSE || collisionLeft || collisionRight) {
            tooClose = true;
            if (millis() - lastBlinkTime > 250) {
                blinkingBright = !blinkingBright;
                lastBlinkTime = millis();
            }

            if (blinkingBright) {
                fill_solid(leds, NUM_LEDS, CRGB::Red);
            } else {
                fill_solid(leds, NUM_LEDS, CRGB::Black);
            }

        } else if (tooClose && (distance > distanceLimit || distance == US_TOO_FAR)) {
            tooClose = false;
            blinkingBright = false;
            // Serial.println("Color changed to normal because distance is now greater than limit.");
            setLEDColor(colorState);
        }
        FastLED.show();
        lastDistanceMeasureTime = millis();
    }
}


void initCollisionDetection() {
  pinMode(COLLISION_LEFT_P, INPUT);
  pinMode(COLLISION_RIGHT_P, INPUT);
}