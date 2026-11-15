#include "myRobot.h"

int volt_index = 0;
int volt_filled = 0;
float voltages[ANZ_AVERAGE_VOLTAGES] = {0};
float lastVoltage = 0;




void initBatteryMeasurement() {
  pinMode(BATTERY_ADC_PIN, INPUT);
}



float robotBatteryVoltage() {
  int adc_value = analogRead(BATTERY_ADC_PIN);

  float real_value = adc_value * 1.47 * VOLTAGE_DEVAITION_FACTOR;  // with respect of the voltage divider, 47k and 100k lead 1.47 and the devaition (different for each robot)
  float battery_voltage = real_value * (3.3 / 4095);

  if (battery_voltage > VOLTAGE_MAX) battery_voltage = VOLTAGE_MAX; 
  /* 
    there is a bug where the battery voltage is measured to high 
    when charged; for not-charging operation normal, therefore it
    is capped to VOLTAGE_MAX (the maximum my cell can reach when being charged)
  */

  voltages[volt_index] = battery_voltage;
  volt_index = (volt_index + 1) % ANZ_AVERAGE_VOLTAGES;

  if (volt_filled < ANZ_AVERAGE_VOLTAGES)
      volt_filled++;

  float sum = 0;
  for (int i = 0; i < volt_filled; i++) {
      sum += voltages[i];
  }

  float voltage_average = sum / volt_filled;
  currentVoltage = voltage_average;
  lastVoltage = currentVoltage;


  return currentVoltage;
}




int calcBattery_Percentage_State() {
    if (currentVoltage <= 0) {
        battery_state = EMPTEY_S;
        Serial.println("Error with voltage - please check!");
        return 0;
    }

    const float realField = VOLTAGE_HIGH - VOLTAGE_LOW;

    int pctSum = 0;

    for (int i = 0; i < volt_filled; i++) {
      const float diff = voltages[i] - VOLTAGE_LOW;
      const int pct = round((diff / realField) * 100);
      pctSum += pct;
    }

    int avgPct = round((float) pctSum / volt_filled);

    if (avgPct > 100) avgPct = 100;
    if (avgPct < 0) avgPct = 0;

    if (avgPct >= BAT_FULL_LIM) battery_state = FULL_S;
    else if (avgPct >= BAT_MEDIUM_LIM) battery_state = MEDIUM_S;
    else if (avgPct >= BAT_LOW_LIM) battery_state = LOW_S;
    else battery_state = EMPTEY_S;

    return avgPct;
}




void loopCallBMS() {
  static uint64_t lastBatteryMeasurement = millis();

  if (millis() - lastBatteryMeasurement >= BMS_REFRESH_TIME) {
    lastBatteryMeasurement = millis();
    robotBatteryVoltage();
    calcBattery_Percentage_State(); // to update the battery state
    // Serial.print("Battery: ");
    // Serial.print(currentVoltage);
    // Serial.println(" %");
  }
}