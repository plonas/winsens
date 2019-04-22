/*
 * ws_ble_wms.c
 *
 *  Created on: 9 cze 2018
 *      Author: Damian.Plonek
 */


#include "ws_ble_wms.h"
#include "nrf_error.h"
#include "app_error.h"
#include "ble_srv_common.h"
#define NRF_LOG_MODULE_NAME "BLE_WMS"
#include "nrf_log.h"

#include <memory.h>


static uint32_t ws_wms_enabled_char_add(ws_ble_wms_t *p_wms);
static uint32_t ws_wms_state_char_add(ws_ble_wms_t *p_wms);
static uint32_t ws_wms_threshold_char_add(ws_ble_wms_t *p_wms);

static void ws_on_connect(ws_ble_wms_t *p_wms, ble_evt_t *p_ble_evt);
static void ws_on_disconnect(ws_ble_wms_t *p_wms, ble_evt_t *p_ble_evt);
static void ws_on_write(ws_ble_wms_t *p_wms, ble_evt_t *p_ble_evt);


uint32_t ws_ble_wms_init(ws_ble_wms_t *p_wms, ws_ble_wms_threshold_write_f on_threshold_write, ws_ble_wms_enabled_write_f on_enabled_write)
{
    uint32_t            err_code;
    ble_uuid_t          service_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_WMS_BASE_UUID;

    NRF_LOG_INFO("ws_ble_wms_init\n");

    p_wms->on_threshold_write = on_threshold_write;
    p_wms->on_enabled_write = on_enabled_write;

    service_uuid.uuid = BLE_UUID_WMS_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &p_wms->service_handle);
    APP_ERROR_CHECK(err_code);

    ws_wms_enabled_char_add(p_wms);
    ws_wms_state_char_add(p_wms);
    ws_wms_threshold_char_add(p_wms);

    return NRF_SUCCESS;
}

uint32_t ws_ble_wms_window_state_update(ws_ble_wms_t *p_wms, ws_ble_wms_state_e state)
{
    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    NRF_LOG_INFO("ws_ble_window_state_update\n");

    if (NULL == p_wms)
    {
        return NRF_ERROR_NULL;
    }

    NRF_LOG_INFO("ws_ble_window_state_update state %hu last state %hu\n", state, p_wms->last_state);

    if (state != p_wms->last_state)
    {
        // Initialize value struct.
        memset(&gatts_value, 0, sizeof(gatts_value));

        gatts_value.len     = sizeof(ws_ble_wms_state_e);
        gatts_value.offset  = 0;
        gatts_value.p_value = &state;

        // Update database.
        err_code = sd_ble_gatts_value_set(p_wms->conn_handle,
            p_wms->state_char_handles.value_handle,
            &gatts_value);

        if (err_code == NRF_SUCCESS)
        {
            // Save new battery value.
            p_wms->last_state = state;
        }
        else
        {
            return err_code;
        }

        if (BLE_CONN_HANDLE_INVALID != p_wms->conn_handle)
        {
            ble_gatts_hvx_params_t hvx_params;

            memset(&hvx_params, 0, sizeof(hvx_params));

            hvx_params.handle = p_wms->state_char_handles.value_handle;
            hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
            hvx_params.offset = gatts_value.offset;
            hvx_params.p_len  = &gatts_value.len;
            hvx_params.p_data = gatts_value.p_value;

            err_code = sd_ble_gatts_hvx(p_wms->conn_handle, &hvx_params);
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
    }

    return err_code;
}

uint32_t ws_ble_wms_enable(ws_ble_wms_t *p_wms, bool enable)
{
    return NRF_SUCCESS;
}

void ws_ble_wms_on_ble_evt(ws_ble_wms_t *p_wms, ble_evt_t *p_ble_evt)
{
    if (p_wms == NULL || p_ble_evt == NULL)
    {
        return;
    }

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            ws_on_connect(p_wms, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            ws_on_disconnect(p_wms, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            ws_on_write(p_wms, p_ble_evt);
            break;

        default:
            NRF_LOG_DEBUG("ws_ble_wms_on_ble_evt event %u\n", p_ble_evt->header.evt_id);
            // No implementation needed.
            break;
    }
}

static uint32_t ws_wms_enabled_char_add(ws_ble_wms_t *p_wms)
{
    //Add a custom characteristic UUID
    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_WMS_BASE_UUID;

    NRF_LOG_INFO("ws_wms_enabled_char_add\n");

    char_uuid.uuid = BLE_UUID_WMS_ENABLED_CHARACTERISTC_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("sd_ble_uuid_vs_add: %lu\n", err_code);

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
    attr_char_value.max_len     = sizeof(uint8_t);
    attr_char_value.init_len    = sizeof(uint8_t);
    attr_char_value.p_value     = (uint8_t*) &p_wms->enabled;

    //Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_wms->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_wms->enabled_char_handles);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("sd_ble_gatts_characteristic_add: %lu\n", err_code);

    return NRF_SUCCESS;
}

static uint32_t ws_wms_state_char_add(ws_ble_wms_t *p_wms)
{
    //Add a custom characteristic UUID
    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_WMS_BASE_UUID;

    NRF_LOG_INFO("ws_wms_state_char_add\n");

    char_uuid.uuid = BLE_UUID_WMS_STATE_CHARACTERISTC_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("sd_ble_uuid_vs_add: %lu\n", err_code);

    //Add read/write properties to our characteristic
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    char_md.char_props.write = 0;

    //Configuring Client Characteristic Configuration Descriptor metadata and add to char_md structure
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;

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
    attr_char_value.max_len     = sizeof(ws_ble_wms_state_e);
    attr_char_value.init_len    = sizeof(ws_ble_wms_state_e);
    attr_char_value.p_value     = (uint8_t*) &p_wms->last_state;

    //Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_wms->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_wms->state_char_handles);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("sd_ble_gatts_characteristic_add: %lu\n", err_code);

    return NRF_SUCCESS;
}

static uint32_t ws_wms_threshold_char_add(ws_ble_wms_t *p_wms)
{
    //Add a custom characteristic UUID
    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_WMS_BASE_UUID;

    NRF_LOG_INFO("ws_wms_threshold_char_add\n");

    char_uuid.uuid = BLE_UUID_WMS_THRESHOLD_CHARACTERISTC_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("sd_ble_uuid_vs_add: %lu\n", err_code);

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
    attr_char_value.p_value     = (uint8_t*) &p_wms->threshold;

    //Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_wms->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_wms->threshold_char_handles);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("sd_ble_gatts_characteristic_add: %lu\n", err_code);

    return NRF_SUCCESS;
}

static void ws_on_connect(ws_ble_wms_t *p_wms, ble_evt_t *p_ble_evt)
{
    p_wms->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

static void ws_on_disconnect(ws_ble_wms_t *p_wms, ble_evt_t *p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_wms->conn_handle = BLE_CONN_HANDLE_INVALID;
}

static void ws_on_write(ws_ble_wms_t *p_wms, ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_write_t *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
//    WS_ASSERT(p_evt_write);

    NRF_LOG_DEBUG("ws_on_write auth_required: %u\n", p_evt_write->auth_required);
    NRF_LOG_DEBUG("ws_on_write data: 0x%x\n", *((uint16_t *) p_evt_write->data));
    NRF_LOG_DEBUG("ws_on_write handle: %u\n", p_evt_write->handle);
    NRF_LOG_DEBUG("ws_on_write len: %u\n", p_evt_write->len);
    NRF_LOG_DEBUG("ws_on_write offset: %u\n", p_evt_write->offset);
    NRF_LOG_DEBUG("ws_on_write op: %u\n", p_evt_write->op);
    NRF_LOG_DEBUG("ws_on_write uuid: 0x%x\n", p_evt_write->uuid.uuid);

    switch (p_evt_write->uuid.uuid)
    {
        case BLE_UUID_WMS_THRESHOLD_CHARACTERISTC_UUID:
            if (p_wms->on_threshold_write)
            {
                p_wms->on_threshold_write(p_wms, *((uint16_t *) p_evt_write->data));
            }
            break;

        case BLE_UUID_WMS_ENABLED_CHARACTERISTC_UUID:
            if (p_wms->on_enabled_write)
            {
                p_wms->on_enabled_write(p_wms, *((uint8_t *) p_evt_write->data));
            }
            break;

        default:
            break;
    }
}
