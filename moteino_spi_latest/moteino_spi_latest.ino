// #include "MAX6675.h"  //  library AUTHOR: Rob Tillaart
#include "MAX31855.h"
#include <stdint.h>
#include "common.h"
#include "globals.h"
#include <LowPower.h>

#define POWER_GPIO 3
// MAX31855 ThermoCouples(5, &SPI);
MAX31855 ThermoCouples[SENSORS_NUM] = {
  MAX31855(K_TYPE_A, &SPI),
  MAX31855(K_TYPE_B, &SPI)
};
// MAX6675 ThermoCouples[SENSORS_NUM] = {
//   MAX6675(K_TYPE_A, &SPI),  //  HW SPI
//   MAX6675(K_TYPE_B, &SPI)   //  HW SPI
//   //MAX6675(5, 6, 7)    //  SW SPI
// };

void setup() {
  //while(!Serial);
  // Serial.begin(SERIAL_BAUD);
  // pinMode(MISO_CLAMP_CTRL_PIN, OUTPUT);

  // During boot, set GPIO HIGH to turn the 2N2222 ON and clamp MISO to GND
  // digitalWrite(MISO_CLAMP_CTRL_PIN, HIGH);
  // Set CS pins high to disable SPI devices
  pinMode(K_TYPE_A, OUTPUT);
  pinMode(K_TYPE_B, OUTPUT);
  pinMode(POWER_GPIO, OUTPUT);
  digitalWrite(POWER_GPIO, HIGH);
  digitalWrite(K_TYPE_A, HIGH);
  digitalWrite(K_TYPE_B, HIGH);


  delay(10);
  SPI.begin();

  // digitalWrite(MISO_CLAMP_CTRL_PIN, LOW);
  for (int i = 0; i < SENSORS_NUM; i++) {
    ThermoCouples[i].begin();
    ThermoCouples[i].setSPIspeed(4000000);
  }


  Radio.initializeRadio();
  // Serial.println("Radio Initialized!");
  Radio.sendConfigPacket(3);
  // Serial.println("Config Packet Sent!");
  // delay(2000);
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}


void loop() {
  // digitalWrite(MISO_CLAMP_CTRL_PIN, LOW);  // clamp MISO again before sleep
  digitalWrite(POWER_GPIO, HIGH);
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);  //1 second power down, to initialize the ktype sensor
  for (int THCnumber = 0; THCnumber < SENSORS_NUM; THCnumber++) {

    ThermoCouples[THCnumber].read();
    // Serial.print("temp ");
    // Serial.print(THCnumber + 1);
    // Serial.print(":\t ");
    // Serial.print(temp);
    // Serial.print("\t ");
  }
  Radio.sendDataPacket(ThermoCouples[0].getTemperature() * 100, ThermoCouples[1].getTemperature() * 100, (uint8_t)0);

  Radio.sleep();
  // Ensure no devices drive MISO during sleep
  digitalWrite(K_TYPE_A, HIGH);
  digitalWrite(K_TYPE_B, HIGH);
  digitalWrite(POWER_GPIO, LOW);
  //  for (int i = 0; i < 38; i++) {
  //   LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); //10 min power down of moteinonot very accurate WDT, actual publish time is 12mins 16secs, 38 is 5 mins
  //  }
   LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);

}
