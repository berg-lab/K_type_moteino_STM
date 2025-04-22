#include "MAX6675.h"  //  library AUTHOR: Rob Tillaart
#include <stdint.h>
#include "common.h"
#include "globals.h"
#define MISO_CLAMP_CTRL_PIN 3  // Choose an available GPIO pin

MAX6675 ThermoCouples[SENSORS_NUM] = {
  MAX6675(K_TYPE_A, &SPI),  //  HW SPI
  MAX6675(K_TYPE_B, &SPI)   //  HW SPI
  //MAX6675(5, 6, 7)    //  SW SPI
};

void setup() {
   //while(!Serial);
  Serial.begin(SERIAL_BAUD);
  pinMode(MISO_CLAMP_CTRL_PIN, OUTPUT);
  
  // During boot, set GPIO HIGH to turn the 2N2222 ON and clamp MISO to GND
  digitalWrite(MISO_CLAMP_CTRL_PIN, HIGH);
  
  delay(3000);  // Wait 3 seconds for the board to boot without interference
  
 
  // Radio Initialization
  Radio.initializeRadio();
  Radio.sendConfigPacket(3);
  Serial.println("Config Packet Sent!");
 
  SPI.begin();
 // After boot, set GPIO LOW to turn the 2N2222 OFF and release MISO
  digitalWrite(MISO_CLAMP_CTRL_PIN, LOW);
  for (int i = 0; i < SENSORS_NUM; i++) {
    ThermoCouples[i].begin();
    ThermoCouples[i].setSPIspeed(4000000);
  }
}


void loop() {


  for (int THCnumber = 0; THCnumber < SENSORS_NUM; THCnumber++) {

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

