/*
 * acc_ctrl.c
 *
 *  Created on: 28.07.2021
 *      Author: Damian Plonek
 */


#include "acc.h"
#include "ble_peripheral.h"
#include "ble_peripheral_cfg.h"
#define ILOG_MODULE_NAME ACCC
#include "log.h"


static void acc_cb(winsens_event_t event);
static void attr_update_cb(ble_peripheral_update_t const *update_data);


static bool     g_initialized   = false;
static uint16_t g_ff_threshold  = 0;
static uint16_t g_hp_threshold  = 0;


LOG_REGISTER();


winsens_status_t acc_ctrl_init(void)
{
    if (!g_initialized)
    {
        g_initialized = true;

        acc_init();
        ble_peripheral_init();

        acc_subscribe(acc_cb);
        ble_peripheral_attr_subscribe(attr_update_cb);

        g_ff_threshold = acc_get_ff_threshold();
        ble_peripheral_update(BLE_PERIPERAL_SVC_ACC, BLE_PERIPERAL_CHAR_ACC_FF, sizeof(g_ff_threshold), (uint8_t*)&g_ff_threshold);
        g_hp_threshold = acc_get_hp_threshold();
        ble_peripheral_update(BLE_PERIPERAL_SVC_ACC, BLE_PERIPERAL_CHAR_ACC_HP, sizeof(g_hp_threshold), (uint8_t*)&g_hp_threshold);
    }

    return WINSENS_OK;
}

static void acc_cb(winsens_event_t event)
{
    switch (event.id)
    {
        case ACC_EVT_FREEFALL_THRESHOLD_CHANGE:
        {
            uint16_t new_threshold = (uint16_t)event.data;
            if (new_threshold != g_ff_threshold)
            {
                g_ff_threshold = new_threshold;
                ble_peripheral_update(BLE_PERIPERAL_SVC_ACC, BLE_PERIPERAL_CHAR_ACC_FF, sizeof(g_ff_threshold), (uint8_t*)&g_ff_threshold);
                LOG_DEBUG("ff threshold update %u", g_ff_threshold);
            }
            break;
        }

        case ACC_EVT_HIPASS_THRESHOLD_CHANGE:
        {
            uint16_t new_threshold = (uint16_t)event.data;
            if (new_threshold != g_hp_threshold)
            {
                g_hp_threshold = new_threshold;
                ble_peripheral_update(BLE_PERIPERAL_SVC_ACC, BLE_PERIPERAL_CHAR_ACC_HP, sizeof(g_hp_threshold), (uint8_t*)&g_hp_threshold);
                LOG_DEBUG("hp threshold update %u", g_hp_threshold);
            }
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
            if (sizeof(g_ff_threshold) == update_data->value_len)
            {
                g_ff_threshold = *(uint16_t*)update_data->value;
                acc_set_ff_threshold(g_ff_threshold);
                LOG_DEBUG("ff threshold update from ble %u", g_ff_threshold);
            }
        }
        else if (BLE_PERIPERAL_CHAR_ACC_HP == update_data->char_id)
        {
            if (sizeof(g_hp_threshold) == update_data->value_len)
            {
                g_hp_threshold = *(uint16_t*)update_data->value;
                acc_set_hp_threshold(g_hp_threshold);
                LOG_DEBUG("hp threshold update from ble %u", g_hp_threshold);
            }
        }
    }
}
