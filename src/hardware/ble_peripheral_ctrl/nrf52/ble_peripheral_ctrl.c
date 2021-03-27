/*
 * ble_peripheral_ctrl.c
 *
 *  Created on: 25.03.2021
 *      Author: Damian Plonek
 */

#include "ble_peripheral_ctrl.h"
#include "ble_peripheral.h"


static void ble_peripheral_evt_handler(winsens_event_t event);


static bool g_bleperipheral_ctrl_initialized = false;


winsens_status_t ble_peripheral_ctrl_init(void)
{
    if (!g_bleperipheral_ctrl_initialized)
    {
        g_bleperipheral_ctrl_initialized = true;

        ble_peripheral_init();
        ble_peripheral_subscribe(ble_peripheral_evt_handler);
    }

    return WINSENS_OK;
}

static void ble_peripheral_evt_handler(winsens_event_t event)
{
    switch (event.id)
    {
        case BLE_PERIPHERAL_EVT_STATE_CHANGE:
        {
            ble_peripheral_state_enum_t state = (ble_peripheral_state_enum_t) event.data;

            if (BLE_PERIPHERAL_STATE_DISCONNECTED == state)
            {
                ble_peripheral_start_advertising();
            }
        } break;

        default:
            break;
    }
}
