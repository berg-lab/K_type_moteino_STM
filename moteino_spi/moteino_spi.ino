//
//    FILE: MAX6675_array.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo

#include "MAX6675.h"
#include <stdint.h>
#include "common.h"
#include "globals.h"
const int ThermoCouplesNum = 2;

MAX6675 ThermoCouples[ThermoCouplesNum] = {
  MAX6675(K_TYPE_A, &SPI),  //  HW SPI
  MAX6675(K_TYPE_B, &SPI)   //  HW SPI
  //MAX6675(5, 6, 7)    //  SW SPI
};

void setup() {
  //  while(!Serial);
  Serial.begin(SERIAL_BAUD);
  Serial.println(__FILE__);
  Serial.print("MAX6675_LIB_VERSION: ");
  Serial.println(MAX6675_LIB_VERSION);
  Serial.println();
  delay(250);
  // Radio Initialization
  Radio.initializeRadio();
  Serial.println("Radio Initialized!");
  Radio.sendConfigPacket(3);
  Serial.println("Config Packet Sent!");
  SPI.begin();

  for (int i = 0; i < ThermoCouplesNum; i++) {
    ThermoCouples[i].begin();
    ThermoCouples[i].setSPIspeed(4000000);
  }
}


void loop() {


  for (int THCnumber = 0; THCnumber < ThermoCouplesNum; THCnumber++) {

    int status = ThermoCouples[THCnumber].read();
    float temp = ThermoCouples[THCnumber].getCelsius();

    
    Serial.print("temp ");
    Serial.print(THCnumber + 1);
    Serial.print(":\t ");
    Serial.print(temp);
    Serial.print("\t ");
    Radio.sendDataPacket(ThermoCouples[0].getCelsius()*100,ThermoCouples[1].getCelsius()*100,(uint8_t) 0);
    delay(500);  //  time to flush all Serial stuff
  }
  Serial.println();
}

//  -- END OF FILE --

