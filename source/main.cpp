/******************************************************************************************
 * Project:   Code m?u co b?n cho Bot trong cu?c thi VNG Bot Battle 12+1
 * Hardware:  
 *    + Main board: Bo m?ch VBLUno51 c?a VNG IoT Lab v?i giao ti?p khong day
 *                  Bluetooth Low Energy (BLE)
 *    + Robot: Day ch? la c?u hinh ph?n c?ng co b?n. Cac d?i tuy bi?n theo nhu c?u
 *        - 2 D?ng co banh sau: Di?u khi?n Ti?n, Lui, Quay trai, Quay ph?i, D?ng
 *        - 4 Relay d? di?u khi?n B?t/T?t cac co c?u
 * Software:  mbed-os  (Da patch cho VBLUNO51)
 *    + Bien d?ch offline b?ng cong c? mbed-cli 
 * Date:      15/7/2017
 * Author:    VNG IoT Lab
 * License:   Apache License 2.0
 ******************************************************************************************/

#define DEBUG_SERIAL                          //S? d?ng khi c?n debug qua c?ng Serial (USB to Serial)


#include <events/mbed_events.h>
#include <mbed.h>
#include "ble/BLE.h"
#include "bot/botservice.h"
#include "utils/utilsprint.h"

#ifdef DEBUG_SERIAL
  Serial pc(USBTX, USBRX);
  #define BAUD_RATE             115200
#endif

#define M1_A                    p28     //D?ng co banh trai, sau
#define M1_B                    p25     //D?ng co banh trai, sau 
#define M2_A                    p24     //D?ng co banh ph?i, sau
#define M2_B                    p23     //D?ng co banh ph?i, sau
#define M3_A                    p22
#define M3_B                    p21
#define M4_A                    p9
#define M4_B                    p16

#define MOTOR_LEFT_A            M1_A
#define MOTOR_LEFT_B            M1_B
#define MOTOR_RIGHT_A           M2_A
#define MOTOR_RIGHT_B           M2_B

#define RL_1                    p17    //Relay 1
#define RL_2                    p18    //Relay 2
#define RL_3                    p19    //Relay 3
#define RL_4                    p20    //Relay 4 

DigitalOut  led(LED, 0);

// Change your device name below
static const char DEVICE_NAME[] = "VNG_Bot2";
static const uint16_t uuid16_list[] = {BotService::bot_service_uuid};
static BotService *bot_service_ptr;
static EventQueue eventQueue(/* event count */ 16 * EVENTS_EVENT_SIZE);


void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params) {
    PRINTF("BLE: Disconnection callback and restart advertising\r\n");
    BLE::Instance().gap().startAdvertising();        // restart advertising
    led = 0;
}

void connectionCallback(const Gap::ConnectionCallbackParams_t *params) {    
    PRINTF("BLE: Connection callback\r\n");
    led = 1;
}

void bleInitComplete(BLE::InitializationCompleteCallbackContext *params) {
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;

    if (error!=0){
        PRINTF("BLE.Init with error: %d \r\n", error);
    }

    if (ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        PRINTF("BLE is not DEFAULT_INSTANCE\r\n");
        return;
    }

    ble.gap().setDeviceName((const uint8_t *)DEVICE_NAME);
    ble.gap().onDisconnection(disconnectionCallback);
    ble.gap().onConnection(connectionCallback);

    /* Setup advertising. */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (const uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().accumulateScanResponse(GapAdvertisingData::COMPLETE_LOCAL_NAME,(const uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(200); 

    /* Setup bot service. */
    bot_service_ptr = new BotService(ble,
                                     (const PinName)MOTOR_LEFT_A,
                                     (const PinName)MOTOR_LEFT_B,
                                     (const PinName)MOTOR_RIGHT_A,
                                     (const PinName)MOTOR_RIGHT_B,
                                     (const PinName)RL_1,
                                     (const PinName)RL_2,
                                     (const PinName)RL_3,
                                     (const PinName)RL_4);

    /* Start advertising */
    error = ble.gap().startAdvertising();
    if (error!=0){
        PRINTF("BLE.startAdvertising with error: %d \r\n", error);
    }

}

void scheduleBleEventsProcessing(BLE::OnEventsToProcessCallbackContext* context) {
    BLE &ble = BLE::Instance();
    eventQueue.call(Callback<void()>(&ble, &BLE::processEvents));
}

int main() {
    //init serial 
#ifdef DEBUG_SERIAL
    pc.baud(BAUD_RATE);
    pc.set_flow_control(SerialBase::RTSCTS, RTS_PIN_NUMBER, CTS_PIN_NUMBER);
    PRINTF_INITIALIZE();
    PRINTF("Start Bot application\r\n");    
#endif

    //init ble
    BLE &ble = BLE::Instance();
    ble.onEventsToProcess(scheduleBleEventsProcessing);
    ble_error_t err = ble.init(bleInitComplete);
    if (err != BLE_ERROR_NONE) {
        PRINTF("BLE initialisation failed");
        return 0;
    }
    
    //init LED
    led = 0;
    
    //Events scheduler
    eventQueue.dispatch_forever();

    return 0;
}
