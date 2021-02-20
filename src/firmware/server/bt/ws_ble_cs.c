/*
 * ws_ble_cs.c
 *
 *  Created on: 16.07.2019
 *      Author: Damian Plonek
 */

#include "server_defs.h"
#include "config_cfg.h"
#include "ws_ble_cs.h"
#include "ws_ble.h"
#define ILOG_MODULE_NAME LECS
#include "log.h"
#include "log_internal_nrf52.h"

#include "ble_srv_common.h"

static uint32_t ws_cs_threshold_char_add(ws_ble_cs_t *p_cs, window_id_t window);
static uint32_t ws_cs_enabled_char_add(ws_ble_cs_t *p_cs, window_id_t window);
static uint32_t ws_cs_apply_char_add(ws_ble_cs_t *p_cs);

static void ws_on_connect(ws_ble_cs_t *p_cs, const ble_evt_t *p_ble_evt);
static void ws_on_disconnect(ws_ble_cs_t *p_cs, const ble_evt_t *p_ble_evt);
static void ws_on_write(ws_ble_cs_t *p_cs, const ble_evt_t *p_ble_evt);


static const server_config_t SERVER_CONFIG_DEFAULT = {false, {400, 400}};


uint32_t ws_ble_cs_init(ws_ble_cs_t *p_cs, ws_ble_cs_threshold_write_f on_threshold_write, ws_ble_cs_enabled_write_f on_enabled_write, ws_ble_cs_enabled_apply_f on_apply_write)
{
    uint8_t             i;
    uint32_t            err_code;
    ble_uuid_t          service_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_WMS_BASE_UUID;

    p_cs->on_threshold_write = on_threshold_write;
    p_cs->on_enabled_write = on_enabled_write;
    p_cs->on_apply_write = on_apply_write;

    service_uuid.uuid = BLE_UUID_CS_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    LOG_NRF_ERROR_RETURN(err_code, err_code);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &p_cs->service_handle);
    LOG_NRF_ERROR_RETURN(err_code, err_code);

    server_config_t server_config;
    config_get(CONFIG_ID_SERVER, &server_config, sizeof(server_config), &SERVER_CONFIG_DEFAULT);

    for (i = 0; i < WINDOW_STATE_CFG_NUMBER; ++i)
    {
        p_cs->enabled[i] = true;
        p_cs->threshold[i] = server_config.window_state_threshold[i];

        err_code = ws_cs_enabled_char_add(p_cs, i);
        LOG_NRF_ERROR_RETURN(err_code, err_code);

        err_code = ws_cs_threshold_char_add(p_cs, i);
        LOG_NRF_ERROR_RETURN(err_code, err_code);
    }

    p_cs->apply = false;
    err_code = ws_cs_apply_char_add(p_cs);
    LOG_NRF_ERROR_RETURN(err_code, err_code);

    return NRF_SUCCESS;
}

void ws_ble_cs_on_ble_evt(ws_ble_cs_t *p_cs, const ble_evt_t *p_ble_evt)
{
    if (p_cs == NULL || p_ble_evt == NULL)
    {
        return;
    }

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            ws_on_connect(p_cs, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            ws_on_disconnect(p_cs, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            ws_on_write(p_cs, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}

static uint32_t ws_cs_threshold_char_add(ws_ble_cs_t *p_cs, window_id_t window)
{
    //Add a custom characteristic UUID
    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_WMS_BASE_UUID;

    char_uuid.uuid = BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_BASE + window;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    LOG_NRF_ERROR_RETURN(err_code, err_code);

    //Add read/write properties to our characteristic
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;

    //Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 0;

    //Configure the attribute metadata
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    //Set read/write security levels to our characteristic
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    //Configure the characteristic value attribute
    ble_gatts_attr_t attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;

    //Set characteristic length in number of bytes
    attr_char_value.max_len     = sizeof(uint16_t);
    attr_char_value.init_len    = sizeof(uint16_t);
    attr_char_value.p_value     = (uint8_t*) &p_cs->threshold[window];

    //Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_cs->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_cs->threshold_char_handles[window]);
    LOG_NRF_ERROR_RETURN(err_code, err_code);

    return NRF_SUCCESS;
}

static uint32_t ws_cs_enabled_char_add(ws_ble_cs_t *p_cs, window_id_t window)
{
    //Add a custom characteristic UUID
    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_WMS_BASE_UUID;

    char_uuid.uuid = BLE_UUID_CS_ENABLED_CHARACTERISTC_UUID_BASE + window;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    LOG_NRF_ERROR_RETURN(err_code, err_code);

    //Add read/write properties to our characteristic
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;

    //Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 0;

    //Configure the attribute metadata
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    //Set read/write security levels to our characteristic
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    //Configure the characteristic value attribute
    ble_gatts_attr_t attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;

    //Set characteristic length in number of bytes
    attr_char_value.max_len     = sizeof(bool);
    attr_char_value.init_len    = sizeof(bool);
    attr_char_value.p_value     = (uint8_t*) &p_cs->enabled[window];

    //Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_cs->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_cs->enabled_char_handles[window]);
    LOG_NRF_ERROR_RETURN(err_code, err_code);

    return NRF_SUCCESS;
}

static uint32_t ws_cs_apply_char_add(ws_ble_cs_t *p_cs)
{
    //Add a custom characteristic UUID
    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_WMS_BASE_UUID;

    char_uuid.uuid = BLE_UUID_CS_APPLY_CHARACTERISTC_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    LOG_NRF_ERROR_RETURN(err_code, err_code);

    //Add read/write properties to our characteristic
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 0;
    char_md.char_props.write = 1;

    //Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 0;

    //Configure the attribute metadata
    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    //Set read/write security levels to our characteristic
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    //Configure the characteristic value attribute
    ble_gatts_attr_t attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid = &char_uuid;
    attr_char_value.p_attr_md = &attr_md;

    //Set characteristic length in number of bytes
    attr_char_value.max_len     = sizeof(bool);
    attr_char_value.init_len    = sizeof(bool);
    attr_char_value.p_value     = (uint8_t*) &p_cs->apply;

    //Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_cs->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_cs->apply_char_handles);
    LOG_NRF_ERROR_RETURN(err_code, err_code);

    return NRF_SUCCESS;
}

static void ws_on_connect(ws_ble_cs_t *p_cs, const ble_evt_t *p_ble_evt)
{
    p_cs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

static void ws_on_disconnect(ws_ble_cs_t *p_cs, const ble_evt_t *p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_cs->conn_handle = BLE_CONN_HANDLE_INVALID;
}

static void ws_on_write(ws_ble_cs_t *p_cs, const ble_evt_t *p_ble_evt)
{
    const ble_gatts_evt_write_t *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
//    UTILS_ASSERT(p_evt_write);

    LOG_DEBUG("ws_on_write auth_required: %u", p_evt_write->auth_required);
    LOG_DEBUG("ws_on_write data: 0x%x", *((uint16_t *) p_evt_write->data));
    LOG_DEBUG("ws_on_write handle: %u", p_evt_write->handle);
    LOG_DEBUG("ws_on_write len: %u", p_evt_write->len);
    LOG_DEBUG("ws_on_write offset: %u", p_evt_write->offset);
    LOG_DEBUG("ws_on_write op: %u", p_evt_write->op);
    LOG_DEBUG("ws_on_write uuid: 0x%x", p_evt_write->uuid.uuid);

    switch (p_evt_write->uuid.uuid)
    {
        case BLE_UUID_CS_ENABLED_CHARACTERISTC_UUID_WIN_1:
            if (p_cs->on_enabled_write)
            {
                p_cs->on_enabled_write(WINDOW_STATE_CFG_WINDOW_LEFT, *((bool *) p_evt_write->data));
            }
            break;
        case BLE_UUID_CS_ENABLED_CHARACTERISTC_UUID_WIN_2:
            if (p_cs->on_enabled_write)
            {
                p_cs->on_enabled_write(WINDOW_STATE_CFG_WINDOW_RIGHT, *((bool *) p_evt_write->data));
            }
            break;
        case BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_WIN_1:
            if (p_cs->on_threshold_write)
            {
                p_cs->on_threshold_write(WINDOW_STATE_CFG_WINDOW_LEFT, *((uint16_t *) p_evt_write->data));
            }
            break;
        case BLE_UUID_CS_THRESHOLD_CHARACTERISTC_UUID_WIN_2:
            if (p_cs->on_threshold_write)
            {
                p_cs->on_threshold_write(WINDOW_STATE_CFG_WINDOW_RIGHT, *((uint16_t *) p_evt_write->data));
            }
            break;
        case BLE_UUID_CS_APPLY_CHARACTERISTC_UUID:
            if (p_cs->on_apply_write)
            {
                p_cs->on_apply_write();
            }
            break;

        default:
            break;
    }
}
