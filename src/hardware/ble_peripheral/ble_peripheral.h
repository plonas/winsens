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


typedef struct
{
    ble_uuid128_t           service_base_uuid;
    ble_uuid_t              service_uuid;
    uint16_t                service_handle;
} ble_peripheral_svc_t;

typedef uint8_t ble_peripheral_svc_id_t;

typedef struct
{
    ble_peripheral_svc_id_t     service_id;
    ble_gatts_char_handles_t    char_handle;
    ble_uuid_t                  char_uuid;
    bool                        read_enabled;
    bool                        write_enabled;
    bool                        notification_enabled;
    uint16_t                    value_len;
} ble_peripheral_char_t;


winsens_status_t ble_peripheral_init(void);

winsens_status_t ble_peripheral_disconnect();

winsens_status_t ble_peripheral_delete_all_peers();

winsens_status_t ble_peripheral_subscribe();

#endif /* BLE_PERIPHERAL_H_ */
