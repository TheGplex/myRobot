#include "myRobot.h"


int lfValues[LF_NUM_VALUES][2];
int lfValueAvg[2];


void initLinefollower() {
    pinMode(LF_RIGHT_LEFT_P, OUTPUT);
    pinMode(LF_ADC_P, INPUT);
    gpio_set_level((gpio_num_t) LF_RIGHT_LEFT_P, LOW);

    memset(lfValues, 0, sizeof(lfValues));
    memset(lfValueAvg, 0, sizeof(lfValueAvg));
}
    
    
void readLfValues() {
    static int lfValuesIndex = 0;
    static int measuredValues = 0; // counts from 0 to LF_NUM_VALUES
    static uint64_t lastLfValueReading = 0;

    if (millis() - lastLfValueReading >= LF_READING_TIME) {
        lastLfValueReading = millis();

        digitalWrite(LF_RIGHT_LEFT_P, HIGH);
        ets_delay_us(1);
        lfValues[lfValuesIndex][LF_VALUE_LEFT] = analogRead(LF_ADC_P) * LF_LEFT_CORRECTION; // Apply correction factor for left linefollower
        digitalWrite(LF_RIGHT_LEFT_P, LOW);
        ets_delay_us(1);
        lfValues[lfValuesIndex][LF_VALUE_RIGHT] = analogRead(LF_ADC_P);

        (++lfValuesIndex) %= LF_NUM_VALUES;
        
        if (measuredValues < LF_NUM_VALUES)
            measuredValues++;
        

        int leftSum = 0, rightSum = 0;

        for (int i = 0; i < measuredValues; i++) {
            leftSum += lfValues[i][LF_VALUE_LEFT];
            rightSum += lfValues[i][LF_VALUE_RIGHT];
        }

        lfValueAvg[LF_VALUE_LEFT] = round((float) leftSum / measuredValues);
        lfValueAvg[LF_VALUE_RIGHT] = round((float) rightSum / measuredValues);
    }
}




void printLfData() {
    static uint64_t lM = 0;
    if (millis() - lM > 500) {
        lM = millis();
        Serial.print("L: ");
        Serial.println(lfValueAvg[LF_VALUE_LEFT]);
        Serial.print("R: ");
        Serial.println(lfValueAvg[LF_VALUE_RIGHT]);
    }
}


void linefollower() {   // v1
    static uint64_t lastLfReading = 0;

    if (millis() - lastLfReading > LF_REFRESH_TIME) {
        lastLfReading = millis();

        int b = 0; // balance, - means to far left, + to far right
        static int bprev = 0; // previous balance

        const int lfL = lfValueAvg[LF_VALUE_LEFT];
        const int lfR = lfValueAvg[LF_VALUE_RIGHT];

        const bool colorOnTrack = isElementOf(colorState, trackcolors, LF_ANZ_TRACKCOLORS);
        const bool leftOnTrack = lfL >= LF_ONTRACK_MIN;
        const bool rightOnTrack = lfR >= LF_ONTRACK_MIN;

        int tempSpeedL = speedL;
        int tempSpeedR = speedR;

        float fadeoutL_mult = 1;
        float fadeoutR_mult = 1;

        if (leftOnTrack && rightOnTrack && colorOnTrack) {
            b = bprev * LF_DAMP_FACTOR;
        } else if (!leftOnTrack && rightOnTrack && colorOnTrack) {
            b = LF_SLIGHTLY_LEFT;
        } else if (!leftOnTrack && rightOnTrack && !colorOnTrack) {
            b = LF_MEDIUM_LEFT;
        } else if (!leftOnTrack && !rightOnTrack && !colorOnTrack && bprev < 0) {
            b = LF_FAR_LEFT;
        } else if (leftOnTrack && !rightOnTrack && colorOnTrack) {
            b = LF_SLIGHTLY_RIGHT;
        } else if (leftOnTrack && !rightOnTrack && !colorOnTrack) {
            b = LF_MEDIUM_RIGHT;
        } else if (!leftOnTrack && !rightOnTrack && !colorOnTrack && bprev > 0) {
            b = LF_FAR_RIGHT;
        } else {
            b = bprev;
        }

        tempSpeedL *= (1 - (float) b / LF_CORRECTION_FACTOR) * fadeoutL_mult;
        tempSpeedR *= (1 + (float) b / LF_CORRECTION_FACTOR) * fadeoutR_mult;

        drive(FORWARD, tempSpeedL, tempSpeedR);
        bprev=b;
    }
}


int isElementOf(int value, const int list[], const int listSize) {
    for (int i = 0; i < listSize; i++) {
        if (value == list[i]) return true;
    }

    return false;
}