/*
 * attr_server_cfg.h
 *
 *  Created on: 27.02.2021
 *      Author: Damian Plonek
 */

#ifndef ATTR_SERVER_CFG_H_
#define ATTR_SERVER_CFG_H_


#define ATTR_SERVER_MAX_CALLBACKS       16

typedef enum
{
    ATTR_SERVER_ID_CS_ENABLED_LEFT,
    ATTR_SERVER_ID_CS_ENABLED_RIGHT,
    ATTR_SERVER_ID_CS_THRESHOLD_LEFT,
    ATTR_SERVER_ID_CS_THRESHOLD_RIGHT,
    ATTR_SERVER_ID_WMS_STATE,

    ATTR_SERVER_ID_NUMBER
} attr_server_attr_id_enum_t;


/*
typedef struct
{
    ble_peripheral_svc_id_t     server_id;
    ble_peripheral_char_id_t    char_id;
} attr_server_config_t;
*/
#define ATTR_SERVER_CONFIG_INIT     { \
        {.server_id = BLE_PERIPERAL_SVC_CS,     .char_id = BLE_PERIPERAL_CHAR_CS_ENABLED_LEFT}, \
        {.server_id = BLE_PERIPERAL_SVC_CS,     .char_id = BLE_PERIPERAL_CHAR_CS_ENABLED_RIGHT}, \
        {.server_id = BLE_PERIPERAL_SVC_CS,     .char_id = BLE_PERIPERAL_CHAR_CS_THRESHOLD_LEFT}, \
        {.server_id = BLE_PERIPERAL_SVC_CS,     .char_id = BLE_PERIPERAL_CHAR_CS_THRESHOLD_RIGHT}, \
        {.server_id = BLE_PERIPERAL_SVC_WMS,    .char_id = BLE_PERIPERAL_CHAR_WMS_STATE}, \
}

#endif /* ATTR_SERVER_CFG_H_ */
