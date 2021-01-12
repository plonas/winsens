/*
 * ws_ble_cs.h
 *
 *  Created on: 16.07.2019
 *      Author: Damian Plonek
 */

#ifndef WS_BLE_CS_H_
#define WS_BLE_CS_H_

#include "winsens_types.h"
#include "IConfig.h"
#include "IWindowState.h"
#include "ble.h"
#include "ble_gatts.h"

#define BLE_UUID_CS_SERVICE_UUID                        0xE00D // Just a random, but recognizable value
#define BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_BASE   0xF000 // Just a random, but recognizable value
#define BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_WIN_1  (BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_BASE + IWINDOW_STATE_CFG_WINDOW_1)
#define BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_WIN_2  (BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_BASE + IWINDOW_STATE_CFG_WINDOW_2)
#define BLE_UUID_CS_ENABLED_CHARACTERISTC_UUID_BASE     0xF010 // Just a random, but recognizable value
#define BLE_UUID_CS_ENABLED_CHARACTERISTC_UUID_WIN_1    (BLE_UUID_CS_ENABLED_CHARACTERISTC_UUID_BASE + IWINDOW_STATE_CFG_WINDOW_1)
#define BLE_UUID_CS_ENABLED_CHARACTERISTC_UUID_WIN_2    (BLE_UUID_CS_ENABLED_CHARACTERISTC_UUID_BASE + IWINDOW_STATE_CFG_WINDOW_2)
#define BLE_UUID_CS_APPLY_CHARACTERISTC_UUID            0xF020 // Just a random, but recognizable value


struct ws_ble_cs;

typedef void (*ws_ble_cs_threshold_write_f)(IWindowId_t window, uint16_t value);
typedef void (*ws_ble_cs_enabled_write_f)(IWindowId_t window, bool value);
typedef void (*ws_ble_cs_enabled_apply_f)(void);

typedef struct ws_ble_cs
{
    uint16_t conn_handle;
    uint16_t service_handle;
    ble_gatts_char_handles_t threshold_char_handles[IWINDOW_STATE_CFG_WINDOWS_NUMBER];
    ble_gatts_char_handles_t enabled_char_handles[IWINDOW_STATE_CFG_WINDOWS_NUMBER];
    ble_gatts_char_handles_t apply_char_handles;

    ws_ble_cs_threshold_write_f on_threshold_write;
    ws_ble_cs_enabled_write_f on_enabled_write;
    ws_ble_cs_enabled_apply_f on_apply_write;

    uint16_t threshold[IWINDOW_STATE_CFG_WINDOWS_NUMBER];
    bool enabled[IWINDOW_STATE_CFG_WINDOWS_NUMBER];
    bool apply;

} ws_ble_cs_t;
#define WS_BLE_CS_INIT                      {BLE_CONN_HANDLE_INVALID, 0x0000, {WS_BLE_CHAR_HANDLES_INIT}, {WS_BLE_CHAR_HANDLES_INIT}, WS_BLE_CHAR_HANDLES_INIT, NULL, NULL, NULL, {0}, {false}, false}

uint32_t ws_ble_cs_init(ws_ble_cs_t *p_cs, const WS_Configuration_t *config, ws_ble_cs_threshold_write_f on_threshold_write, ws_ble_cs_enabled_write_f on_enabled_write, ws_ble_cs_enabled_apply_f on_apply_write);

void ws_ble_cs_on_ble_evt(ws_ble_cs_t *p_cs, const ble_evt_t *p_ble_evt);

#endif /* WS_BLE_CS_H_ */
