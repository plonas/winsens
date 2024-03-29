/*
 * ble_peripheral.h
 *
 *  Created on: 20.02.2021
 *      Author: Damian Plonek
 */

#ifndef BLE_PERIPHERAL_H_
#define BLE_PERIPHERAL_H_


#include "ble_gatts.h"
#include "ble_types.h"
#include "winsens_types.h"


#define BLE_PERIPHERAL_CHAR_ID_INVALID      (0xFFFF)


typedef enum
{
    BLE_PERIPHERAL_EVT_STATE_CHANGE = (WINSENS_IDS_MODULE_BLE_PERIPHERAL << 16),

} ble_peripheral_evt_t;

typedef enum
{
    BLE_PERIPHERAL_STATE_UNKNOWN,
    BLE_PERIPHERAL_STATE_CONNECTED,
    BLE_PERIPHERAL_STATE_CONNECTING,
    BLE_PERIPHERAL_STATE_DISCONNECTED,
    BLE_PERIPHERAL_STATE_DISCONNECTING,
    BLE_PERIPHERAL_STATE_ADVERTISING,
    BLE_PERIPHERAL_STATE_BONDING,
    BLE_PERIPHERAL_STATE_UNBONDING,

} ble_peripheral_state_enum_t;

typedef struct
{
    ble_uuid128_t           service_base_uuid;
    ble_uuid_t              service_uuid;
    uint16_t                service_handle;
} ble_peripheral_svc_t;

typedef uint16_t ble_peripheral_svc_id_t;
typedef uint16_t ble_peripheral_char_id_t;

typedef struct
{
    ble_peripheral_svc_id_t     service_id;
    ble_gatts_char_handles_t    char_handle;
    ble_uuid_t                  char_uuid;
    bool                        read_enabled;
    bool                        write_enabled;
    bool                        notifications_enabled;
    bool                        notifications_active;
    uint16_t                    value_len;
    char                        *desc;
} ble_peripheral_char_t;

typedef struct
{
    ble_peripheral_svc_id_t     server_id;
    ble_peripheral_char_id_t    char_id;
    uint16_t                    value_len;
    uint8_t const               *value;
} ble_peripheral_update_t;

typedef void (*ble_peripheral_attr_cb_t) (ble_peripheral_update_t const *update_data);


winsens_status_t ble_peripheral_init(void);

winsens_status_t ble_peripheral_disconnect(void);

winsens_status_t ble_peripheral_start_advertising(void);

winsens_status_t ble_peripheral_delete_all_peers(void);

winsens_status_t ble_peripheral_bond(void);
winsens_status_t ble_peripheral_unbond(void);

winsens_status_t ble_peripheral_subscribe(winsens_event_handler_t evt_handler);

winsens_status_t ble_peripheral_attr_subscribe(ble_peripheral_attr_cb_t callback);
winsens_status_t ble_peripheral_update(ble_peripheral_svc_id_t server_id, ble_peripheral_char_id_t char_id, uint16_t value_len, uint8_t *value);

ble_peripheral_state_enum_t ble_peripheral_get_state(void);

#endif /* BLE_PERIPHERAL_H_ */
