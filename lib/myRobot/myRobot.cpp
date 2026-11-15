#include "myRobot.h"

bool speedAdjusted = false;
bool tooClose = false;
bool collisionLeft = false, collisionRight = false;
uint8_t speedL, speedR;
uint8_t adjustedSpeedL, adjustedSpeedR;
uint8_t reductionSpeedL, reductionSpeedR;


// BATTERY MEASUREMENT
float currentVoltage = 0;
int battery_state = 0;




void initDriving() {
    pinMode(MR_SPEED_P, OUTPUT);
    pinMode(MR_DIR_P, OUTPUT);
    pinMode(ML_SPEED_P, OUTPUT);
    pinMode(ML_DIR_P, OUTPUT);
    

    ledcSetup(MR_CHPWM, 5000, 8);
    ledcSetup(ML_CHPWM, 5000, 8);

    ledcAttachPin(MR_SPEED_P, MR_CHPWM);
    ledcAttachPin(ML_SPEED_P, ML_CHPWM);

    drive(STOP, 0, 0);
}


void drive(const int mode, const int speedLeft, const int speedRight) {
    if (mode == STOP) {
        ledcWrite(ML_CHPWM, 0);
        ledcWrite(MR_CHPWM, 0);
        return;
    }

    int rightState = (mode == FORWARD || mode == TURN_LEFT ? HIGH : LOW);
    int leftState = (mode == BACKWARD || mode == TURN_LEFT ? HIGH : LOW);

    gpio_set_level((gpio_num_t) ML_DIR_P, leftState);
    gpio_set_level((gpio_num_t) MR_DIR_P, rightState);
    ledcWrite(ML_CHPWM, speedLeft);
    ledcWrite(MR_CHPWM, speedRight);
}


void initUltrasonicSensor() {
    pinMode(US_TRIGGER_P, OUTPUT);
    pinMode(US_ECHO_P, INPUT);
    gpio_set_level((gpio_num_t) US_TRIGGER_P, LOW);
}


float getDistanceCM() {
    digitalWrite(US_TRIGGER_P, HIGH);
    delayMicroseconds(10);
    digitalWrite(US_TRIGGER_P, LOW);
    
    int duration = pulseIn(US_ECHO_P, HIGH, 30000); // 30ms timeout (30 ms ~ 5m ~ US_MAX_RANGE)
    float distance = duration * SONIC_SPEED_CMPUS / 2;
    // Serial.println(distance);

    if (distance <= US_MIN_RANGE && distance != 0) {
        return US_TOO_CLOSE;
    } else if (distance > US_MAX_RANGE || distance == 0) {
        return US_TOO_FAR;
    }

    return distance;
}

void initDriveAdjustment() {
    pinMode(PULSE_GENERATOR_LEFT_P, INPUT);
    pinMode(PULSE_GENERATOR_RIGHT_P, INPUT);
}

void driveAdjustment() {
    static bool leftChecked = false, rightChecked = false;
    static uint32_t pulsesLeft = 0, pulsesRight = 0;
    static uint64_t adjustmentStartTime = 0;

    if (!leftChecked && digitalRead(PULSE_GENERATOR_LEFT_P) == 0) {
        pulsesLeft++;
        leftChecked = true;
    }

    if (leftChecked && digitalRead(PULSE_GENERATOR_LEFT_P)) {
        leftChecked = false;
    }

    if (!rightChecked && digitalRead(PULSE_GENERATOR_RIGHT_P) == 0) {
        pulsesRight++;
        rightChecked = true;
    }

    if (rightChecked && digitalRead(PULSE_GENERATOR_RIGHT_P)) {
        rightChecked = false;
    }


    if (millis() - adjustmentStartTime > 20) {
        adjustmentStartTime = millis();

        bool leftSlower = false, rightSlower = false;
        uint8_t oldAdjSpeedL = adjustedSpeedL, oldAdjSpeedR = adjustedSpeedR;

        if (pulsesLeft > pulsesRight) {
            rightSlower = true;
        } else if (pulsesRight > pulsesLeft) {
            leftSlower = true;
        } else {
            return;
        }

        if (leftSlower) {
            reductionSpeedR = round(6 * (float) speedL / 100);
            reductionSpeedL = 0;
        } else if (rightSlower) {
            reductionSpeedR = 0;
            reductionSpeedL = round(1 * (float) speedR / 100);
        }

        adjustedSpeedL = speedL - reductionSpeedL;
        adjustedSpeedR = speedR - reductionSpeedR;

        if (adjustedSpeedL != oldAdjSpeedL || adjustedSpeedR != oldAdjSpeedR) {
            speedAdjusted = true;
        }
    }
}



void collisionDetection() {
  if (!digitalRead(COLLISION_LEFT_P)) {
    collisionLeft = true;
    Serial.println("COLLISION LEFT SIDE !!!");
  } else {
    collisionLeft = false;
  }

  if (!digitalRead(COLLISION_RIGHT_P)) {
    collisionRight = true;
    Serial.println("COLLISION RIGHT SIDE !!!");
  } else {
    collisionRight = false;
  }

}


#ifdef USE_LEDS
CRGB leds[NUM_LEDS] = {0};

void initLEDs(int brightness) {
    FastLED.addLeds<LED_TYPE, DATA_P, CLOCK_P, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(brightness);
}
#endif

