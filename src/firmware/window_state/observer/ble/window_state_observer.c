/*
 * window_state_observer.c
 *
 *  Created on: 02.09.20201
 *      Author: Damian.Plonek
 */

#include "ble_peripheral.h"
#include "ble_peripheral_cfg.h"
#include "window_state_observer.h"
#include "window_state.h"
#define ILOG_MODULE_NAME win_state_obs
#include "log.h"


void win_state_evt_handler(winsens_event_t evt);


LOG_REGISTER();


winsens_status_t window_state_observer_ble_init(void)
{
    winsens_status_t status = ble_peripheral_init();
    LOG_ERROR_RETURN(status, status);

    status = window_state_init();
    LOG_ERROR_RETURN(status, status);

    status = window_state_subscribe(win_state_evt_handler);
    LOG_ERROR_RETURN(status, status);

    return WINSENS_OK;
}

void win_state_evt_handler(winsens_event_t evt)
{
    uint8_t value = (uint8_t)window_state_get(evt.data);
    ble_peripheral_update(BLE_PERIPERAL_SVC_WMS, BLE_PERIPERAL_CHAR_WMS_STATE, sizeof(value), &value);
}
