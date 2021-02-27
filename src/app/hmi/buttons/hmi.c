/*
 * hmi.c
 *
 *  Created on: 27.02.2021
 *      Author: Damian Plonek
 */


#include "ble_peripheral.h"
#include "button.h"
#include "digital_io_cfg.h"


static void button_callback(winsens_event_t event);


winsens_status_t hmi_init(void)
{
    ble_peripheral_init();
    button_init();

    button_register_callback(DIGITAL_IO_INPUT_PAIR_BTN, button_callback);

    return WINSENS_OK;
}

static void button_callback(winsens_event_t event)
{
    if (BUTTON_EVENT_NORMAL == event.id)
    {
        const ble_peripheral_state_enum_t ble_state = ble_peripheral_get_state();

        if (BLE_PERIPHERAL_STATE_CONNECTED == ble_state)
        {
            ble_peripheral_bond();
        }
        else if (BLE_PERIPHERAL_STATE_DISCONNECTED == ble_state)
        {
            ble_peripheral_start_advertising();
        }
    }
    else if (BUTTON_EVENT_VERY_LONG == event.id)
    {
        ble_peripheral_unbond();
    }
}