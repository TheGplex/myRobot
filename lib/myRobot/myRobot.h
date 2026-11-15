#ifndef MYROBOT_H
#define MYROBOT_H


#include "Arduino.h"
#include "syscore.h"




/**********************************************************/
// LEDS
/**********************************************************/

#ifdef USE_LEDS
#include <FastLED.h>

#define NUM_LEDS    4
#define LED_TYPE    SK9822
#define COLOR_ORDER RBG
#define DATA_P    23
#define CLOCK_P   18

extern CRGB leds[NUM_LEDS];
#endif





/**********************************************************/
// COLORSENSOR
/**********************************************************/

#ifdef USE_COLORSENSOR
#include <Adafruit_TCS34725.h>

#define ENABLE_COLOR_OUTPUT false

#define R_OFFSET (0.2)
#define G_OFFSET (0.15)
#define B_OFFSET (-0.16)

#define LED_COLORSENSOR_P 13

#define STATE_RED 0
#define STATE_GREEN 1
#define STATE_BLUE 2
#define STATE_WHITE 3
#define STATE_BLACK 4
#define STATE_UNKNOWN 5

#define INTEGRATION_TIME TCS34725_INTEGRATIONTIME_101MS
#define TCS_GAIN TCS34725_GAIN_4X

#define R 0
#define G 1
#define B 2
#define C 3

#define TSC_MEASURE_WAIT_TIME 24 // in ms | IMPORTANT: INTEGRATION_TIME behaves like a delay, so these two times add up
#define PRINT_COLOR_TIME 1000 // 1s | The time between two prints of the color

#define WHITE_MIN 20000 // all C values of/above WHITE_MIN are detected as white
#define BLACK_MAX 5000 // all C values of/below BLACK_MAX are detected as black

#define RED_CLEAR 1.5 // if R ist more than RED_CLEAR times higher than the other values, red is detected
#define GREEN_CLEAR 1.2 // if G ist more than GREEN_CLEAR times higher than the other values, green is detected
#define BLUE_CLEAR 1.2 // if B ist more than BLUE_CLEAR times higher than the other values, blue is detected

extern Adafruit_TCS34725 tcs;
extern volatile uint16_t tscColor[4];
extern int colorState;
extern uint64_t lastMeasureStart;
#endif





/**********************************************************/
// LINEFOLLOWER
/**********************************************************/

#ifdef USE_LINEFOLLOWER
#define LF_RIGHT_LEFT_P 12 // Linefollower Pin (Right/Left)
#define LF_ADC_P 36 // Linefollower Pin (ADC1 Pin0)
#define LF_VALUE_LEFT 0 // Value for the left linefollower
#define LF_VALUE_RIGHT 1 // Value for the right linefollower
#define LF_LEFT_CORRECTION 1.18 // Correction factor for the left linefollower
#define LF_NUM_VALUES 10
#define LF_READING_TIME 20 // ms, Lf values are measured every LF_READING_TIME ms

#define LF_ONTRACK_MIN 2000 // the mimimum value to still be detected as "on-track"
#define LF_REFRESH_TIME 20 // in ms, how often the direction is calculated

extern int lfValues[LF_NUM_VALUES][2]; // Storage for the last 3 values of the left and right linefollower
extern int lfValueAvg[2]; // Storage for the median value of the left and right linefollower

#define LF_ANZ_TRACKCOLORS 4
const int trackcolors[LF_ANZ_TRACKCOLORS] = {STATE_BLACK, STATE_RED, STATE_GREEN, STATE_BLUE};

#define LF_FAR_LEFT (-4)
#define LF_MEDIUM_LEFT (-3)
#define LF_SLIGHTLY_LEFT (-2)
#define LF_BALANCED 0
#define LF_SLIGHTLY_RIGHT 2
#define LF_MEDIUM_RIGHT 3
#define LF_FAR_RIGHT 4

#define LF_CORRECTION_FACTOR 5
#define LF_DAMP_FACTOR 0.7
#endif





/**********************************************************/
// PS5 CONTROLLER
/**********************************************************/




// yet to build




/**********************************************************/
// WEBSERVER
/**********************************************************/

#ifdef USE_WEBSERVER
#include <ESPAsyncWebServer.h>
#endif





/**********************************************************/
// NORMAL DEFINES
/**********************************************************/

#define ML_SPEED_P 32   // Motor Left Speed Pin
#define ML_DIR_P   33   // Motor Left Direction Pin
#define MR_SPEED_P 2    // Motor Right Speed Pin
#define MR_DIR_P   15   // Motor Right Direction Pin

#define ML_CHPWM 1 // PWM Channel for Motor Left
#define MR_CHPWM 0 // PWM Channel for Motor Right

//IMPORTANT: Pullup Resistor is included, therfore the pin is LOW when a collision is detected -> pinMode INPUT not INPUT_PULLUP
#define COLLISION_LEFT_P 34 // Collision Sensor Left Pin (only the one close to the middle works fine)
#define COLLISION_RIGHT_P 35 // Collision Sensor Right Pin (no one works fine, a change would be great)
//ENDIMPORTANT

#define US_TRIGGER_P 25 // Trigger Pin for the Ultrasonic Sensor
#define US_ECHO_P 26    // Echo Pin for the Ultrasonic Sensor
#define SONIC_SPEED_CMPUS (34345.0 / 1000000.0) // Speed of sound in cm/us (34345 cm/us = 343.450 m/s)
#define US_MAX_RANGE 400 // Maximum range of the ultrasonic sensor in cm
#define US_TOO_FAR (-1)
#define US_MIN_RANGE 3   // Minimum range of the ultrasonic sensor in cm
#define US_TOO_CLOSE (-2)

//IMPORTANT: Pullup Resistor is included, therfore the pin is LOW when the wheel moved -> pinMode INPUT not INPUT_PULLUP
#define PULSE_GENERATOR_LEFT_P 27 // Pulse Generator Left Pin (Gabellichtschranke)   (left is pin 14 in schematic, but actually pin 27 is next to the left motor, not pin 14)
#define PULSE_GENERATOR_RIGHT_P 14 // Pulse Generator Right Pin (Gabellichtschranke) (right is pin 27 in schematic, but actually pin 14 is next to the right motor, not pin 27)
//ENDIMPORTANT

#define BATTERY_ADC_PIN 39
#define ANZ_AVERAGE_VOLTAGES 100
#define VOLTAGE_TOLERANCE 0.01
#define VOLTAGE_DEVAITION_FACTOR 1.0675
#define VOLTAGE_LOW 3.2
#define VOLTAGE_HIGH 4.1
#define VOLTAGE_MAX 4.15
#define BMS_REFRESH_TIME 250 // ms

#define BAT_FULL_LIM 80 // battery is detected as full at 80% or higher
#define BAT_MEDIUM_LIM 40 // see above
#define BAT_LOW_LIM 15 // see above




enum BATTERY_STATES {
    FULL_S,
    MEDIUM_S,
    LOW_S,
    EMPTEY_S
};


enum DRIVE_DIRECTIONS {
    FORWARD,
    BACKWARD,
    TURN_LEFT,
    TURN_RIGHT,
    STOP
};




extern bool tooClose;
extern bool collisionLeft, collisionRight;

extern bool speedAdjusted;
extern uint8_t speedL, speedR;
extern uint8_t adjustedSpeedL, adjustedSpeedR;
extern uint8_t reductionSpeedL, reductionSpeedR;

extern float currentVoltage;
extern int battery_state;





/**********************************************************/
// NORMAL FUNCTIONS
/**********************************************************/

void initDriving();
void drive(int mode, int speedLeft, int speedRight);

void initDriveAdjustment();
void driveAdjustment();

void initUltrasonicSensor();
float getDistanceCM();

void collisonWarning(int distanceLimit); // tells you when you are close to colliding
void initCollisionDetection();
void collisionDetection(); // tells you when you collided

void initBatteryMeasurement();
float robotBatteryVoltage();
int calcBattery_Percentage_State();
void loopCallBMS();





/**********************************************************/
// LIBRARY-DEPENDENT FUNCTIONS
/**********************************************************/

#ifdef USE_LEDS
void initLEDs(int brightness);
#endif



#ifdef USE_COLORSENSOR
void initColorSensor(int ledState);
void calculateColor();
void setLEDColor(int colorState);
void getRawDataInstant(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c);

// IMPORTANT: YOU MAY ONLY ACCESS THE COLOR VALUES IN THIS FUNCTION, NOT OUTSIDE OF IT
void measureTSC();
#endif



#ifdef USE_LINEFOLLOWER
void initLinefollower();
void readLfValues();
void printLfData();
int isElementOf(int value, const int list[], const int listSize);
void linefollower();
#endif


#ifdef USE_WEBSERVER
void initWebServer();
String getBatteryPercentageString();
String getBatteryStateString();
String getBatteryLevelString();
String getLfValuesString();
#endif


#endif // MYROBOT_H