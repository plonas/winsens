/*
 * ws_ble_wms.h
 *
 *  Created on: 9 cze 2018
 *      Author: Damian.Plonek
 */

#ifndef WS_BLE_WMS_H_
#define WS_BLE_WMS_H_

#include "ws_ble.h"
#include "ble.h"
#include "ble_gatts.h"

#include <stdint.h>
#include <stdbool.h>

#define BLE_UUID_WMS_SERVICE_UUID                   0xF00D // Just a random, but recognizable value
#define BLE_UUID_WMS_STATE_CHARACTERISTC_UUID       0xBEEF // Just a random, but recognizable value


struct ws_ble_wms;

typedef enum
{
    WS_BLE_WMS_STATE_UNKNOWN,
    WS_BLE_WMS_STATE_OPEN,
    WS_BLE_WMS_STATE_CLOSED,

} ws_ble_wms_state_e;

typedef struct ws_ble_wms
{
    uint16_t conn_handle;
    uint16_t service_handle;
    ble_gatts_char_handles_t state_char_handles;

    ws_ble_wms_state_e last_state;

} ws_ble_wms_t;
#define WS_BLE_WMS_INIT                     {BLE_CONN_HANDLE_INVALID, 0x0000, WS_BLE_CHAR_HANDLES_INIT, WS_BLE_WMS_STATE_UNKNOWN}

uint32_t ws_ble_wms_init(ws_ble_wms_t *p_wms);

uint32_t ws_ble_wms_window_state_update(ws_ble_wms_t *p_wms, ws_ble_wms_state_e state);

void ws_ble_wms_on_ble_evt(ws_ble_wms_t *p_wms, const ble_evt_t *p_ble_evt);

#endif /* WS_BLE_WMS_H_ */
