#ifndef __VNG_BOT_SERVICE_H__
#define __VNG_BOT_SERVICE_H__

#include "ble/BLE.h"
#include "utils/utilsprint.h"

class BotService {
public:

    //UUIDs
    const static uint16_t bot_service_uuid        = 0xFFF0;
    const static uint16_t bot_motion_char_uuid    = 0xFFF1;
    const static uint16_t bot_relay1_char_uuid    = 0xFFF2;
    const static uint16_t bot_relay2_char_uuid    = 0xFFF3;
    const static uint16_t bot_relay3_char_uuid    = 0xFFF4;
    const static uint16_t bot_relay4_char_uuid    = 0xFFF5;

    //BLE Appearance
    const static uint16_t bot_appearance          = 6000;
    
    enum {
        MOTION_STOP = 0, 
        MOTION_FORWARD,     
        MOTION_BACK,     
        MOTION_LEFT,   
        MOTION_RIGHT,         
    };
    
    enum {
        STOP = 0,
        DIR_FORWARD = 1,
        DIR_BACK = 2,
    };
    
    enum {
        RELAY_ON = 0,
        RELAY_OFF = 1,
    };

public:

    /**
     * @brief Constructor for Bot Service
     *
     * @param[in] _ble
     *               Reference to the underlying BLE.
     */
    BotService( BLE &_ble, 
                const PinName _motor_left_a,
                const PinName _motor_left_b,
                const PinName _motor_right_a,
                const PinName _motor_right_b,
                const PinName _relay1,
                const PinName _relay2,
                const PinName _relay3,
                const PinName _relay4) :
        ble(_ble),        
        motor_left_a(_motor_left_a),
        motor_left_b(_motor_left_b),
        motor_right_a(_motor_right_a),
        motor_right_b(_motor_right_b),
        relay1(_relay1),
        relay2(_relay2),
        relay3(_relay3),
        relay4(_relay4),
        bot_motion_char(bot_motion_char_uuid, &bot_motion_char_value), 
        bot_relay1_char(bot_relay1_char_uuid, &bot_relay1_char_value),
        bot_relay2_char(bot_relay2_char_uuid, &bot_relay2_char_value),
        bot_relay3_char(bot_relay3_char_uuid, &bot_relay3_char_value),
        bot_relay4_char(bot_relay4_char_uuid, &bot_relay4_char_value) {
        
        setupService();
    }

    /**
     * This callback allows the heart rate service to receive updates to the
     * controlPoint characteristic.
     *
     * @param[in] params
     *     Information about the characterisitc being updated.
     */
    virtual void onDataWritten(const GattWriteCallbackParams *params) {
        if (params->handle == bot_motion_char.getValueAttribute().getHandle()) {
            switch(params->data[0]) {
                case MOTION_FORWARD: {
                    goForward();
                    break;
                }
                
                case MOTION_BACK: {
                    goBack();
                    break;
                }
                
                case MOTION_LEFT: {
                    rotateLeft();
                    break;
                }
                
                case MOTION_RIGHT: {
                    rotateRight();
                    break;
                }
                
                default: {
                    stop();
                    break;
                }
            }//sw          
        }//if
        
        else if (params->handle == bot_relay1_char.getValueAttribute().getHandle()) {
            controlRelay1(params->data[0]);
        }
        
        else if (params->handle == bot_relay2_char.getValueAttribute().getHandle()) {
            controlRelay2(params->data[0]);
        }
        
        else if (params->handle == bot_relay3_char.getValueAttribute().getHandle()) {
            controlRelay3(params->data[0]);
        }
        
        else if (params->handle == bot_relay4_char.getValueAttribute().getHandle()) {
            controlRelay4(params->data[0]);
        }
    }

protected:
    void setupService(void) {
        GattCharacteristic *bot_chars[] = {&bot_motion_char,
                                           &bot_relay1_char,
                                           &bot_relay2_char,
                                           &bot_relay3_char,
                                           &bot_relay4_char};
        GattService         bot_service(bot_service_uuid, 
                                        bot_chars, 
                                        sizeof(bot_chars) / sizeof(GattCharacteristic *));

        ble.addService(bot_service);
        ble.gattServer().onDataWritten(this, &BotService::onDataWritten);
    }
    
    void controlMotor(uint8_t left_dir, uint8_t right_dir) {
        //motor left
        switch(left_dir) {
            case DIR_FORWARD: {
                motor_left_a = 1;
                motor_left_b = 0;              
                break;
            }
            case DIR_BACK: {
                motor_left_a = 0;
                motor_left_b = 1;  
                break;
            }
            default: {
                motor_left_a = 1;
                motor_left_b = 1;  
                break;
            }
        }
        
        //motor right
        switch(right_dir) {
            case DIR_FORWARD: {
                motor_right_a = 1;
                motor_right_b = 0;    
                break;
            }
            case DIR_BACK: {
                motor_right_a = 0;
                motor_right_b = 1;    
                break;
            }
            default: {
                motor_right_a = 1;
                motor_right_b = 1;    
                break;
            }
        }
    }
    
    void stop(){
        controlMotor(STOP, STOP);
        PRINTF("Stop\r\n");
    }
    
    void rotateLeft(){
        controlMotor(DIR_BACK, DIR_FORWARD);  
        PRINTF("Rotate Left\r\n");
    }
    
    void rotateRight(){
        controlMotor(DIR_FORWARD, DIR_BACK);  
        PRINTF("Rotate Right\r\n");
    }
    
    void goForward(){
        controlMotor(DIR_FORWARD, DIR_FORWARD); 
        PRINTF("Go Forward\r\n");
    }

    void goBack(){
        controlMotor(DIR_BACK, DIR_BACK);
        PRINTF("Go Back\r\n");
    }
    
    void controlRelay1(uint8_t value) {        
        relay1 = value;
        if (value == RELAY_ON)
            PRINTF("Turn relay 1 ON\r\n");
        else
            PRINTF("Turn relay 1 OFF\r\n");
    }
    
    void controlRelay2(uint8_t value) {        
        relay2 = value;
        if (value == RELAY_ON)
            PRINTF("Turn relay 2 ON\r\n");
        else
            PRINTF("Turn relay 2 OFF\r\n");
    }
    
    void controlRelay3(uint8_t value) {        
        relay3 = value;
        if (value == RELAY_ON)
            PRINTF("Turn relay 3 ON\r\n");
        else
            PRINTF("Turn relay 3 OFF\r\n");
    }
    
    void controlRelay4(uint8_t value) {        
        relay4 = value;
        if (value == RELAY_ON)
            PRINTF("Turn relay 4 ON\r\n");
        else
            PRINTF("Turn relay 4 OFF\r\n");
    }

protected:
    BLE                 &ble;
    
    uint8_t              bot_motion_char_value;
    uint8_t              bot_relay1_char_value;
    uint8_t              bot_relay2_char_value;
    uint8_t              bot_relay3_char_value;
    uint8_t              bot_relay4_char_value;
        
    WriteOnlyGattCharacteristic<uint8_t> bot_motion_char;
    WriteOnlyGattCharacteristic<uint8_t> bot_relay1_char;
    WriteOnlyGattCharacteristic<uint8_t> bot_relay2_char;
    WriteOnlyGattCharacteristic<uint8_t> bot_relay3_char;
    WriteOnlyGattCharacteristic<uint8_t> bot_relay4_char;
    
    DigitalOut relay1;
    DigitalOut relay2;
    DigitalOut relay3;
    DigitalOut relay4;    
    
    DigitalOut motor_left_a;
    DigitalOut motor_left_b;
    DigitalOut motor_right_a;
    DigitalOut motor_right_b;
};

#endif /* #ifndef __VNG_BOT_SERVICE_H__*/
