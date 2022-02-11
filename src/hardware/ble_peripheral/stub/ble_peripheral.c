/*
 * ble_peripheral.c
 *
 *  Created on: 06.02.2022
 *      Author: Damian Plonek
 */

#include "ble_peripheral.h"
#define ILOG_MODULE_NAME SVBT
#include "log.h"
#include "log_internal_nrf52.h"


typedef enum
{
    BLE_PERIPHERAL_EVT_CONNECT,
    BLE_PERIPHERAL_EVT_CONNECTED,
    BLE_PERIPHERAL_EVT_DISCONNECT,
    BLE_PERIPHERAL_EVT_DISCONNECTED,
    BLE_PERIPHERAL_EVT_ADVERTISE,
    BLE_PERIPHERAL_EVT_ADVERTISING_STARTED,
    BLE_PERIPHERAL_EVT_ADVERTISING_STOPPED,
    BLE_PERIPHERAL_EVT_WRITE,

    BLE_PERIPHERAL_EVT_UNBOND,
    BLE_PERIPHERAL_EVT_UNBONDED,
    BLE_PERIPHERAL_EVT_BOND,
    BLE_PERIPHERAL_EVT_BONDED,

} ble_peripheral_evt_enum_t;

typedef void (*ble_peripheral_evt_handler_t)(winsens_event_t event);
typedef void (*ble_peripheral_state_entry_t)(void);
typedef void (*ble_peripheral_state_exit_t)(void);

typedef struct WS_ServerBtState_s
{
    ble_peripheral_state_enum_t     state_id;
    ble_peripheral_evt_handler_t    evt_handler;
    ble_peripheral_state_entry_t    enter_state;
    ble_peripheral_state_exit_t     exit_state;

} ble_peripheral_state_t;


LOG_REGISTER();


static bool g_initialized = false;


winsens_status_t ble_peripheral_init(void)
{
    winsens_status_t status = WINSENS_OK;

    if (false == g_initialized)
    {
        g_initialized = true;
    }

    return status;
}

winsens_status_t ble_peripheral_disconnect()
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return WINSENS_OK;
}

winsens_status_t ble_peripheral_start_advertising(void)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return WINSENS_OK;
}

winsens_status_t ble_peripheral_delete_all_peers()
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return WINSENS_OK;
}

winsens_status_t ble_peripheral_bond(void)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return WINSENS_OK;
}

winsens_status_t ble_peripheral_unbond(void)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return WINSENS_OK;
}

winsens_status_t ble_peripheral_subscribe(winsens_event_handler_t evt_handler)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return WINSENS_OK;
}

winsens_status_t ble_peripheral_attr_subscribe(ble_peripheral_attr_cb_t callback)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return WINSENS_OK;
}

winsens_status_t ble_peripheral_update(ble_peripheral_svc_id_t server_id, ble_peripheral_char_id_t char_id, uint16_t value_len, uint8_t *value)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return WINSENS_OK;
}

ble_peripheral_state_enum_t ble_peripheral_get_state(void)
{
    return BLE_PERIPHERAL_EVT_DISCONNECTED;
}
