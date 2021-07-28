/*
 * acc_ctrl.c
 *
 *  Created on: 28.07.2021
 *      Author: Damian Plonek
 */


#include "acc.h"
#include "ble_peripheral.h"
#include "ble_peripheral_cfg.h"


static void acc_cb(winsens_event_t event);
static void attr_update_cb(ble_peripheral_update_t const *update_data);


static bool     g_initialized   = false;


winsens_status_t acc_ctrl_init(void)
{
    if (!g_initialized)
    {
        g_initialized = true;

        acc_init();
        ble_peripheral_init();

        acc_subscribe(acc_cb);
        ble_peripheral_attr_subscribe(attr_update_cb);
    }

    return WINSENS_OK;
}

static void acc_cb(winsens_event_t event)
{
    switch (event.id)
    {
        case ACC_EVT_FREEFALL_THRESHOLD_CHANGE:
        {
            uint16_t threshold = (uint16_t)event.data;
            ble_peripheral_update(BLE_PERIPERAL_SVC_ACC, BLE_PERIPERAL_CHAR_ACC_FF, sizeof(threshold), (uint8_t*)&threshold);
            break;
        }

        case ACC_EVT_HIPASS_THRESHOLD_CHANGE:
        {
            uint16_t threshold = (uint16_t)event.data;
            ble_peripheral_update(BLE_PERIPERAL_SVC_ACC, BLE_PERIPERAL_CHAR_ACC_HP, sizeof(threshold), (uint8_t*)&threshold);
            break;
        }

        default:
            break;
    }
}

static void attr_update_cb(ble_peripheral_update_t const *update_data)
{
    if (BLE_PERIPERAL_SVC_ACC == update_data->server_id)
    {
        if (BLE_PERIPERAL_CHAR_ACC_FF == update_data->char_id)
        {
            uint16_t ff_threshold = 0;
            if (sizeof(ff_threshold) == update_data->value_len)
            {
                ff_threshold = *(uint16_t*)update_data->value;
                acc_set_ff_threshold(ff_threshold);
            }
        }
        else if (BLE_PERIPERAL_CHAR_ACC_HP == update_data->char_id)
        {
            uint16_t hp_threshold = 0;
            if (sizeof(hp_threshold) == update_data->value_len)
            {
                hp_threshold = *(uint16_t*)update_data->value;
                acc_set_hp_threshold(hp_threshold);
            }
        }
    }
}
