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


static uint32_t ws_wms_char_add(ws_ble_wms_t * p_wms);


uint32_t ws_ble_wms_init(ws_ble_wms_t *p_wms)
{
    uint32_t            err_code;
    ble_uuid_t          service_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_WMS_BASE_UUID;

    NRF_LOG_INFO("ws_ble_wms_init\n");

    service_uuid.uuid = BLE_UUID_WMS_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &p_wms->service_handle);
    APP_ERROR_CHECK(err_code);

    ws_wms_char_add(p_wms);
    return NRF_SUCCESS;
}

uint32_t ws_ble_window_state_update(ws_ble_wms_t *p_wms, bool status)
{
    return NRF_SUCCESS;
}

static uint32_t ws_wms_char_add(ws_ble_wms_t * p_wms)
{
    //Add a custom characteristic UUID
    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_WMS_BASE_UUID;

    NRF_LOG_INFO("ws_wms_char_add\n");

    char_uuid.uuid = BLE_UUID_WMS_CHARACTERISTC_UUID;
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
    attr_char_value.max_len     = 4;
    attr_char_value.init_len    = 4;
    uint8_t value[4]            = {0x12,0x34,0x56,0x78};
    attr_char_value.p_value     = value;

    //Add our new characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_wms->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_wms->char_handles);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("sd_ble_gatts_characteristic_add: %lu\n", err_code);

    return NRF_SUCCESS;
}
