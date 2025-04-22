#ifndef _COMMON_H_
#define _COMMON_H_


// Define the current serial port ----------------------------------
#if defined (MOTEINO_M0) && defined (SERIAL_PORT_USBVIRTUAL)
  #define Serial SERIAL_PORT_USBVIRTUAL // Required for Serial on Zero based boards
#endif


// Permanent globals -----------------------------------------------
#define FW_VERSION                1
#define TELEMETRY_VERSION         1
#define DEVICE_TYPE               2 // 1 denotes BORC, 2 denotes STM

// Global configuration constants ----------------------------------
#define SERIAL_BAUD               115200
#define MAX_VOLTAGE               5.0f
#define MAX_ADC                   4095
#define ACK_TIMEOUT               10


// Pin mapping -----------------------------------------------------
#define K_TYPE_A                  5       // change these based on your requirement (digital pins)
#define K_TYPE_B                  6 

//Node specific
#define SENSORS_NUM               2        // increase /decrease based on requirement
#define NODE_ID                   54      //change for each node

// Error bits ------------------------------------------------------
#define BATT_ERR                  0   // battery low/bad voltage
#define TEMP_SENSE_ERR            1   // temp sensor not working
#define CURRENT_SENSE_ERR         3   // current sensor not working
#define RADIO_ERR                 6   // Radio error

#endif
