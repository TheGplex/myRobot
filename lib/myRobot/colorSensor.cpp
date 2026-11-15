#include "myRobot.h"


Adafruit_TCS34725 tcs = Adafruit_TCS34725(INTEGRATION_TIME, TCS_GAIN);
volatile uint16_t tscColor[4] = {0, 0, 0, 0};
int colorState = STATE_UNKNOWN;
uint64_t lastMeasureStart = millis();

void initColorSensor(int ledState) {
    tcs.begin();
    pinMode(LED_COLORSENSOR_P, OUTPUT);
    gpio_set_level((gpio_num_t) LED_COLORSENSOR_P, ledState);
    getRawDataInstant((uint16_t*) &tscColor[R], (uint16_t*) &tscColor[G], (uint16_t*) &tscColor[B], (uint16_t*) &tscColor[C]);
    lastMeasureStart = millis();
}


void calculateColor() {
    tscColor[R] /= (float) (1 + R_OFFSET);
    tscColor[G] /= (float) (1 + G_OFFSET);
    tscColor[B] /= (float) (1 + B_OFFSET);

    if (ENABLE_COLOR_OUTPUT) {
        Serial.print("R: ");
        Serial.print(tscColor[R]);
        Serial.print(" | G: ");
        Serial.print(tscColor[G]);
        Serial.print(" | B: ");
        Serial.print(tscColor[B]);
        Serial.print(" | C: ");
        Serial.println(tscColor[C]);
    }

    if (tscColor[R] > tscColor[G] * RED_CLEAR && tscColor[R] > tscColor[B] * RED_CLEAR) {
        colorState = STATE_RED;
    } else if (tscColor[G] > tscColor[R] * GREEN_CLEAR && tscColor[G] > tscColor[B] * GREEN_CLEAR) {
        colorState = STATE_GREEN;
    } else if (tscColor[B] > tscColor[R] * BLUE_CLEAR && tscColor[B] > tscColor[G] * BLUE_CLEAR) {
        colorState = STATE_BLUE;
    } else if (tscColor[C] >= WHITE_MIN) {
        colorState = STATE_WHITE;
    } else if (tscColor[C] <= BLACK_MAX) {
        colorState = STATE_BLACK;
    } else {
        colorState = STATE_UNKNOWN;
    }
}


void setLEDColor(int colorState) {
    // Serial.print("Color: "); 
        switch (colorState) {
            case STATE_RED:
                // Serial.println("Red");
                fill_solid(leds, NUM_LEDS, CRGB::Red);
                break;
            case STATE_GREEN:
                // Serial.println("Green");
                fill_solid(leds, NUM_LEDS, CRGB::Green);
                break;
            case STATE_BLUE:
                // Serial.println("Blue");
                fill_solid(leds, NUM_LEDS, CRGB::Blue);
                break;
            case STATE_WHITE:
                // Serial.println("White");
                fill_solid(leds, NUM_LEDS, CRGB::White);
                break;
            case STATE_BLACK:
                // Serial.println("Black");
                fill_solid(leds, NUM_LEDS, CRGB::Black);
                break;
            default:
                // Serial.println("Unknown");
                fill_solid(leds, NUM_LEDS, CRGB::Yellow);
                break;
        }
        
        if (!tooClose) {
            FastLED.show();
        }
}


void getRawDataInstant(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c) {
  *c = tcs.read16(TCS34725_CDATAL);
  *r = tcs.read16(TCS34725_RDATAL);
  *g = tcs.read16(TCS34725_GDATAL);
  *b = tcs.read16(TCS34725_BDATAL);
}


void measureTSC() {
    if (millis() - lastMeasureStart >= TSC_MEASURE_WAIT_TIME) {
        calculateColor();

        // HERE YOU CAN ACCESS THE COLOR VALUES, NO WHERE ELSE!!!

        lastMeasureStart = millis();
        getRawDataInstant((uint16_t*) &tscColor[R], (uint16_t*) &tscColor[G], (uint16_t*) &tscColor[B], (uint16_t*) &tscColor[C]);
    }
}