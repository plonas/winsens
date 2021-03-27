/*
 * hmi.c
 *
 *  Created on: 27.02.2021
 *      Author: Damian Plonek
 */


#include "ble_peripheral.h"
#include "button.h"
#include "digital_io_cfg.h"
#define ILOG_MODULE_NAME HMI
#include "log.h"


static void button_callback(winsens_event_t event);


LOG_REGISTER();


winsens_status_t hmi_init(void)
{
    ble_peripheral_init();
    button_init();

    button_register_callback(DIGITAL_IO_INPUT_PAIR_BTN, button_callback);

    return WINSENS_OK;
}

static void button_callback(winsens_event_t event)
{
    const ble_peripheral_state_enum_t ble_state = ble_peripheral_get_state();

    if (BUTTON_EVENT_NORMAL == event.id)
    {
        if (BLE_PERIPHERAL_STATE_CONNECTED == ble_state)
        {
            ble_peripheral_bond();
        }
        else if (BLE_PERIPHERAL_STATE_DISCONNECTED == ble_state)
        {
            ble_peripheral_start_advertising();
        }
    }
    else if (BUTTON_EVENT_LONG == event.id)
    {
        if (BLE_PERIPHERAL_STATE_CONNECTED == ble_state)
        {
            ble_peripheral_disconnect();
        }
        else if (BLE_PERIPHERAL_STATE_DISCONNECTED == ble_state)
        {
            ble_peripheral_unbond();
        }
    }
}
