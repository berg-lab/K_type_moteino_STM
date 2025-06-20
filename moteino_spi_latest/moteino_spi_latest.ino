// #include "MAX6675.h"  //  library AUTHOR: Rob Tillaart
#include "MAX31855.h"
#include <stdint.h>
#include "common.h"
#include "globals.h"
#include <LowPower.h>

#define MISO_CLAMP_CTRL_PIN 3  // Choose an available GPIO pin

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
  Serial.begin(SERIAL_BAUD);
  pinMode(MISO_CLAMP_CTRL_PIN, OUTPUT);

  // During boot, set GPIO HIGH to turn the 2N2222 ON and clamp MISO to GND
  digitalWrite(MISO_CLAMP_CTRL_PIN, HIGH);

  Radio.initializeRadio();
  Serial.println("Radio Initialized!");
  Radio.sendConfigPacket(3);
  Serial.println("Config Packet Sent!");
  delay(2000);

  SPI.begin();
  digitalWrite(MISO_CLAMP_CTRL_PIN, LOW);
  for (int i = 0; i < SENSORS_NUM; i++) {
    ThermoCouples[i].begin();
    ThermoCouples[i].setSPIspeed(4000000);
  }
}


void loop() {


  for (int THCnumber = 0; THCnumber < SENSORS_NUM; THCnumber++) {

    int status = ThermoCouples[THCnumber].read();
    float temp = ThermoCouples[THCnumber].getTemperature();


    Serial.print("temp ");
    Serial.print(THCnumber + 1);
    Serial.print(":\t ");
    Serial.print(temp);
    Serial.print("\t ");
  }
  Radio.sendDataPacket(ThermoCouples[0].getTemperature() * 100, ThermoCouples[1].getTemperature() * 100, (uint8_t)0);
  // delay(500);  //  time to flush all Serial stuff
  Serial.println();

  Serial.flush();
  //low power sleep for 8s
  // LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  LowPower.longPowerDown(8000);  //sleep # of milliseconds, for 10 mins (600000), 8000 = 8s
}
