/*
 * ble_peripheral_cfg.h
 *
 *  Created on: 27.02.2021
 *      Author: Damian Plonek
 */

#ifndef BLE_PERIPHERAL_CFG_H_
#define BLE_PERIPHERAL_CFG_H_


#define BLE_PERIPERAL_MAX_CALLBACKS                     3
#define BLE_PERIPERAL_MAX_EVT_HANDLERS                  3

#define BLE_UUID_WMS_BASE_UUID                          {{0x23, 0xD1, 0x13, 0xEF, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}} // 128-bit base UUID
#define BLE_UUID_CS_SERVICE_UUID                        0xE00D // Just a random, but recognizable value
#define BLE_UUID_WMS_SERVICE_UUID                       0xF00D // Just a random, but recognizable value
#define BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_BASE   0xF000 // Just a random, but recognizable value
#define BLE_UUID_CS_ENABLED_CHARACTERISTC_UUID_BASE     0xF010 // Just a random, but recognizable value
#define BLE_UUID_WMS_STATE_CHARACTERISTC_UUID           0xBEEF // Just a random, but recognizable value

typedef enum
{
    BLE_PERIPERAL_SVC_CS,
    BLE_PERIPERAL_SVC_WMS,
} ble_peripheral_svc_id_enum_t;

typedef enum
{
    BLE_PERIPERAL_CHAR_CS_ENABLED_LEFT,
    BLE_PERIPERAL_CHAR_CS_ENABLED_RIGHT,
    BLE_PERIPERAL_CHAR_CS_THRESHOLD_LEFT,
    BLE_PERIPERAL_CHAR_CS_THRESHOLD_RIGHT,
    BLE_PERIPERAL_CHAR_WMS_STATE,
} ble_peripheral_char_id_enum_t;

/*
typedef struct
{
    ble_uuid128_t           service_base_uuid;
    ble_uuid_t              service_uuid;
    uint16_t                service_handle;
} ble_peripheral_svc_t;
*/
#define BLE_PERIPHERAL_SERVICES_INIT    { \
    { \
    .service_base_uuid = BLE_UUID_WMS_BASE_UUID, \
    .service_uuid = {.uuid = BLE_UUID_CS_SERVICE_UUID} \
    }, \
    { \
    .service_base_uuid = BLE_UUID_WMS_BASE_UUID, \
    .service_uuid = {.uuid = BLE_UUID_WMS_SERVICE_UUID} \
    } \
}

/*
typedef struct
{
    ble_peripheral_svc_id_t     service_id;
    ble_gatts_char_handles_t    char_handle;
    ble_uuid_t                  char_uuid;
    bool                        read_enabled;
    bool                        write_enabled;
    bool                        notification_enabled;
    uint16_t                    value_len;
    uint8_t                     *value;
} ble_peripheral_char_t;
*/
#define BLE_PERIPHERAL_CHARS_INIT       { \
    { \
    .service_id = BLE_PERIPERAL_SVC_CS, \
    .char_uuid = {.uuid = BLE_UUID_CS_ENABLED_CHARACTERISTC_UUID_BASE + WINDOW_STATE_CFG_WINDOW_LEFT}, \
    .read_enabled = true , \
    .write_enabled = true, \
    .notification_enabled = false, \
    .value_len = sizeof(bool), \
    .desc = "Enable left window", \
    }, \
    { \
    .service_id = BLE_PERIPERAL_SVC_CS, \
    .char_uuid = {.uuid = BLE_UUID_CS_ENABLED_CHARACTERISTC_UUID_BASE + WINDOW_STATE_CFG_WINDOW_RIGHT}, \
    .read_enabled = true , \
    .write_enabled = true, \
    .notification_enabled = false, \
    .value_len = sizeof(bool), \
    .desc = "Enable right window", \
    }, \
    { \
    .service_id = BLE_PERIPERAL_SVC_CS, \
    .char_uuid = {.uuid = BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_BASE + WINDOW_STATE_CFG_WINDOW_LEFT}, \
    .read_enabled = true , \
    .write_enabled = true, \
    .notification_enabled = false, \
    .value_len = sizeof(uint16_t), \
    .desc = "Left window's threshold", \
    }, \
    { \
    .service_id = BLE_PERIPERAL_SVC_CS, \
    .char_uuid = {.uuid = BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_BASE + WINDOW_STATE_CFG_WINDOW_RIGHT}, \
    .read_enabled = true , \
    .write_enabled = true, \
    .notification_enabled = false, \
    .value_len = sizeof(uint16_t), \
    .desc = "Right window's threshold", \
    }, \
    { \
    .service_id = BLE_PERIPERAL_SVC_CS, \
    .char_uuid = {.uuid = BLE_UUID_WMS_STATE_CHARACTERISTC_UUID}, \
    .read_enabled = true , \
    .write_enabled = false, \
    .notification_enabled = true, \
    .value_len = sizeof(uint8_t), \
    .desc = "Window's state", \
    }, \
}


#endif /* BLE_PERIPHERAL_CFG_H_ */
