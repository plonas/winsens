/*
 * ws_ble_wms.h
 *
 *  Created on: 9 cze 2018
 *      Author: Damian.Plonek
 */

#ifndef WS_BLE_WMS_H_
#define WS_BLE_WMS_H_

#include "ble_gatts.h"

#include <stdint.h>
#include <stdbool.h>

#define BLE_UUID_WMS_BASE_UUID              {{0x23, 0xD1, 0x13, 0xEF, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}} // 128-bit base UUID
#define BLE_UUID_WMS_SERVICE_UUID           0xF00D // Just a random, but recognizable value
#define BLE_UUID_WMS_CHARACTERISTC_UUID     0xBEEF // Just a random, but recognizable value


typedef struct
{
    uint16_t conn_handle;
    uint16_t service_handle;
    ble_gatts_char_handles_t char_handles;

} ws_ble_wms_t;
#define WS_BLE_WMS_INIT                     {BLE_CONN_HANDLE_INVALID, 0x0000, {BLE_GATT_HANDLE_INVALID, BLE_GATT_HANDLE_INVALID, BLE_GATT_HANDLE_INVALID, BLE_GATT_HANDLE_INVALID}}

uint32_t ws_ble_wms_init(ws_ble_wms_t *p_wms);

uint32_t ws_ble_window_state_update(ws_ble_wms_t *p_wms, bool status);

#endif /* WS_BLE_WMS_H_ */
