/*
 * battery_observer.c
 *
 *  Created on: 20.11.20201
 *      Author: Damian.Plonek
 */

#include "ble_peripheral.h"
#include "ble_peripheral_cfg.h"
#include "battery_observer.h"
#include "window_state_observer_cfg.h"
#include "battery.h"
#define ILOG_MODULE_NAME bat_obr
#include "log.h"


static void battery_evt_handler(winsens_event_t evt);
static void update_level(void);


LOG_REGISTER();


winsens_status_t battery_observer_init(void)
{
    winsens_status_t status = ble_peripheral_init();
    LOG_ERROR_RETURN(status, status);

    status = battery_init();
    LOG_ERROR_RETURN(status, status);

    status = battery_subscribe(battery_evt_handler);
    LOG_ERROR_RETURN(status, status);

    update_level();

    return WINSENS_OK;
}

void battery_evt_handler(winsens_event_t evt)
{
    update_level();
}

void update_level(void)
{
    battery_level_t value = 0;

    winsens_status_t status = battery_get_level(&value);
    LOG_WARNING_RETURN(status, ;);

    status = ble_peripheral_update(BLE_PERIPERAL_SVC_BAT, BLE_PERIPERAL_CHAR_BAT_LVL, sizeof(value), &value);
    LOG_ERROR_RETURN(status, ;);
}
