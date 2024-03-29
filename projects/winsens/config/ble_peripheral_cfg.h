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
#define BLE_UUID_CS_SERVICE_UUID                        0xA001 // Just a random, but recognizable value
#define BLE_UUID_WMS_SERVICE_UUID                       0xA002 // Just a random, but recognizable value
#define BLE_UUID_ACC_SERVICE_UUID                       0xA003 // Just a random, but recognizable value
#define BLE_UUID_BAT_SERVICE_UUID                       0xA004 // Just a random, but recognizable value

#define BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_BASE   0xF000 // Just a random, but recognizable value
#define BLE_UUID_WMS_STATE_CHARACTERISTC_UUID           0xF020 // Just a random, but recognizable value
#define BLE_UUID_ACC_FF_SENSITIV_CHARACTERISTC_UUID     0xF030 // Just a random, but recognizable value
#define BLE_UUID_ACC_HP_SENSITIV_CHARACTERISTC_UUID     0xF040 // Just a random, but recognizable value
#define BLE_UUID_BAT_LEVEL_CHARACTERISTC_UUID           0xF050 // Just a random, but recognizable value
#define BLE_UUID_DISTANCE_CHARACTERISTC_UUID            0xF060 // Just a random, but recognizable value

typedef enum
{
    BLE_PERIPERAL_SVC_CS,
    BLE_PERIPERAL_SVC_WMS,
    BLE_PERIPERAL_SVC_ACC,
    BLE_PERIPERAL_SVC_BAT,
} ble_peripheral_svc_id_enum_t;

typedef enum
{
    BLE_PERIPERAL_CHAR_CS_THRESHOLD_WINDOW,
    BLE_PERIPERAL_CHAR_WMS_STATE,
    BLE_PERIPERAL_CHAR_ACC_FF,
    BLE_PERIPERAL_CHAR_ACC_HP,
    BLE_PERIPERAL_CHAR_BAT_LVL,
    BLE_PERIPERAL_CHAR_DISTANCE,
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
    }, \
    { \
    .service_base_uuid = BLE_UUID_WMS_BASE_UUID, \
    .service_uuid = {.uuid = BLE_UUID_ACC_SERVICE_UUID} \
    }, \
    { \
    .service_base_uuid = BLE_UUID_WMS_BASE_UUID, \
    .service_uuid = {.uuid = BLE_UUID_BAT_SERVICE_UUID} \
    }, \
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
    bool                        notifications_active;
    uint16_t                    value_len;
    uint8_t                     *value;
} ble_peripheral_char_t;
*/
#define BLE_PERIPHERAL_CHARS_INIT       { \
    { \
    .service_id = BLE_PERIPERAL_SVC_CS, \
    .char_uuid = {.uuid = BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_BASE + WINDOW_STATE_CFG_WINDOW}, \
    .read_enabled = true , \
    .write_enabled = true, \
    .notifications_enabled = false, \
    .notifications_active = false, \
    .value_len = sizeof(int16_t), \
    .desc = "Window's threshold", \
    }, \
    { \
    .service_id = BLE_PERIPERAL_SVC_WMS, \
    .char_uuid = {.uuid = BLE_UUID_WMS_STATE_CHARACTERISTC_UUID}, \
    .read_enabled = true , \
    .write_enabled = false, \
    .notifications_enabled = false, \
    .notifications_active = true, \
    .value_len = sizeof(uint8_t), \
    .desc = "Window's state", \
    }, \
    { \
    .service_id = BLE_PERIPERAL_SVC_ACC, \
    .char_uuid = {.uuid = BLE_UUID_ACC_FF_SENSITIV_CHARACTERISTC_UUID}, \
    .read_enabled = true , \
    .write_enabled = true, \
    .notifications_enabled = false, \
    .notifications_active = false, \
    .value_len = sizeof(uint16_t), \
    .desc = "Free fall sensitivity", \
    }, \
    { \
    .service_id = BLE_PERIPERAL_SVC_ACC, \
    .char_uuid = {.uuid = BLE_UUID_ACC_HP_SENSITIV_CHARACTERISTC_UUID}, \
    .read_enabled = true , \
    .write_enabled = true, \
    .notifications_enabled = false, \
    .notifications_active = false, \
    .value_len = sizeof(uint16_t), \
    .desc = "High pass sensitivity", \
    }, \
    { \
    .service_id = BLE_PERIPERAL_SVC_BAT, \
    .char_uuid = {.uuid = BLE_UUID_BAT_LEVEL_CHARACTERISTC_UUID}, \
    .read_enabled = true , \
    .write_enabled = false, \
    .notifications_enabled = false, \
    .notifications_active = true, \
    .value_len = sizeof(uint8_t), \
    .desc = "Battery level", \
    }, \
    { \
    .service_id = BLE_PERIPERAL_SVC_CS, \
    .char_uuid = {.uuid = BLE_UUID_DISTANCE_CHARACTERISTC_UUID}, \
    .read_enabled = true , \
    .write_enabled = false, \
    .notifications_enabled = false, \
    .notifications_active = true, \
    .value_len = sizeof(uint16_t), \
    .desc = "Distance", \
    }, \
}


#endif /* BLE_PERIPHERAL_CFG_H_ */
