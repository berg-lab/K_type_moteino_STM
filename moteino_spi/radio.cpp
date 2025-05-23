#include "HardwareSerial.h"
//=========================================================================================================
// radio.cpp - Implements a radio interface to the gateway
//=========================================================================================================
#include "radio.h"
#include "globals.h"
#include "common.h"
#include "mstimer.h"
// Define radio communication variables
#define WITHRFM69
#ifdef WITHRFM69
  #include <RFM69.h>
  #include <RFM69_ATC.h>

  #ifdef ENABLE_ATC
    RFM69_ATC radio;
  #else
    RFM69 radio;
  #endif
  #define RADIO_FREQ RF69_915MHZ
#endif
#define IS_RFM69HW
//=========================================================================================================
// maps a structure onto a buffer named "raw"
//=========================================================================================================
#define map_struct(t,m) t& m=*(t*)raw
//=========================================================================================================
msTimer      m_timer;

//=========================================================================================================
// device_config_t  - This is a config packet we transmit to the gateway when we are in configuration mode
//
// stm_trap_data_t  - This is a telemetry packet we transmit to the gateway every time we wake up from our
//                    sleep cycle (or when the Serial Server "transmit" command is received)
//  
// from_gateway_t   - This contains bit flags that inform the node on what fields need to be updated.
//                    If no bits are set, it's an acknowledgement message
//=========================================================================================================
struct device_config_t
{
    uint8_t     pkt_type;
    uint8_t     version;
    uint8_t     device_type;
    uint16_t    firmware_version;
    uint8_t     uid[8];
};

struct stm_trap_telemetry_t
{
    uint8_t     pkt_type;
    uint8_t     version;
    uint16_t    pre_temperature;
    uint16_t    post_temperature;
    uint8_t     error_byte;
    uint8_t     transaction_id;
  
};

struct from_gateway_t
{
    uint8_t     pkt_type;
    uint8_t     tasks_bit_field;
    uint8_t     network_id;
    uint16_t    node_id;
    uint8_t     encryption_key[16];
};
//=========================================================================================================

// radio packet type
enum pkt_type_t  : uint8_t
{
    CONFIG_PACKET       = 0,
    TELEMETRY_PACKET    = 1,
    RESPONSE_PACKET     = 2
};

//=========================================================================================================
// initializeRadio() - Start up radio function and report if error occurs
//=========================================================================================================
void CRadio::initializeRadio() {

    #ifdef WITHRFM69
        // if hardware permits, try radio object initialization
        if (!radio.initialize(RADIO_FREQ, NODE_ID, NETWORK_ID))
            Error.raise_error(RADIO_ERR);
        else {
            Error.clear_error(RADIO_ERR);
            radio.setHighPower();
            Serial.println("Radio initialized!");
        }
        //if hardware permits, try setting high power
        // #ifdef IS_RFM69HW
        // printf("high power.....");
        //     radio.setHighPower(); //uncomment only for RFM69HW!
        // #endif
    #endif

    // assign encription key to radio object for later transmission
    radio.encrypt(ENCRYPT_KEY);
    radio.sleep();

    // start the transaction id at 1
    transaction_id = 1;
}
//=========================================================================================================


//=========================================================================================================
// sendDataPacket() - Loads data into packet and transmits it over the radio
//=========================================================================================================
void CRadio::sendDataPacket(uint16_t pre_temp,uint16_t post_temp, uint8_t error_byte)
{
    stm_trap_telemetry_t telemetry;
    stm_trap_telemetry_t* p = &telemetry;
    bool success = false;

    // Fill in the data in the telemetry packet
   telemetry.pkt_type           = TELEMETRY_PACKET;
    telemetry.version           = TELEMETRY_VERSION;
    telemetry.pre_temperature   = pre_temp;
    telemetry.post_temperature  = post_temp;
    telemetry.error_byte        = error_byte;
    telemetry.transaction_id    = transaction_id;
    
    // Serial.println(pre_temp);
    //   Serial.println(post_temp);
    
    // Attempt to send a packet to the gateway 3 times and wait for response
    for (int attempts = 0; attempts < 3; ++attempts)
    {
        // send the packet
        radio.send(GATEWAY_ID, p, sizeof(telemetry));

       // start a 1 second timer
        m_timer.start(500);


        // sit in a loop for 1 second until a response is received
        while (!m_timer.is_expired())
        {
            // if we receive a response
            if (radio.receiveDone())
            {
                // handle response and break out
                handleIncomingPacket(radio.DATA);
                success = true;
                break;
            }
        }
        
        if (success)  break;
    }

    // increment transaction ID and reset it to 1 if it overflows
    if (++transaction_id == 0) transaction_id = 1;
}
//=========================================================================================================


//=========================================================================================================
// sendConfigPacket() - Transmits a config packet to the gateway to initialize a new node
//=========================================================================================================
void CRadio::sendConfigPacket(int no_of_attempts)
{   
    device_config_t device_config;
    bool success = false;

    // Fill in the node config details in the telemetry packet
    device_config.pkt_type          = CONFIG_PACKET;
    device_config.device_type       = DEVICE_TYPE;
    device_config.version           = TELEMETRY_VERSION;
    device_config.firmware_version  = FW_VERSION;
    uint8_t temp2[8] = {0,0,0,0,0,0,0,0};
    memcpy(temp2, device_config.uid, sizeof(uint8_t)*8);
    
    // Attempt to send a packet to the gateway 3 times and wait for response
    for (int attempt = 0; attempt < no_of_attempts; ++attempt)
    {
        Serial.print("Attempt: ");
        Serial.println(attempt);
        // send the prepared config packet
        radio.send(GATEWAY_ID, &device_config, sizeof(device_config));

     
        // start a 1 second timer
        m_timer.start(500);

        //sit in a loop for 1 second until a response is received
        while (!m_timer.is_expired())
        {
            // if we receive a response
            if (radio.receiveDone())
            {
                // handle response and break out
                handleIncomingPacket(radio.DATA);
                success = true;
                break;
            }
        }
        if (success)  break;
    }
    Serial.println("Finally done");
}
//=========================================================================================================


//=========================================================================================================
// handle_incoming_radio_packet() - Handles an incoming radio packet as ACK only
//=========================================================================================================
void CRadio::handleIncomingPacket(const unsigned char* raw)
{    
    // use the map_struct macro to map the from_gateway_t 
    map_struct(from_gateway_t, packet);

    // make sure the packet type is a response from the gateway to BORC
    if (packet.pkt_type == RESPONSE_PACKET)
    {   
        // blink the LED fast to indicate we a received packet remove this later
        for (int x=0; x<3; x++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(25);
            digitalWrite(LED_BUILTIN, LOW);
            delay(25);
        }
    }
}
//=========================================================================================================
