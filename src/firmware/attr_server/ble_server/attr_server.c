/*
 * attr_server.c
 *
 *  Created on: 27.02.2021
 *      Author: Damian Plonek
 */

#include "attr_server.h"
#include "attr_server_cfg.h"
#include "ble_peripheral.h"
#include "ble_peripheral_cfg.h"


#define ATTRIBUTES_NUMBER           (sizeof(g_attr_server_config)/sizeof(attr_server_config_t))


typedef struct
{
    ble_peripheral_svc_id_t     service_id;
    ble_peripheral_char_id_t    char_id;
} attr_server_config_t;


void update_char_cb(ble_peripheral_update_t const *update_data);

void update_subscribers(attr_server_attr_id_t attr_id, attr_server_value_t value);
void update_ble_server(attr_server_attr_id_t attr_id, attr_server_value_t value);
attr_server_attr_id_t get_attr_id(ble_peripheral_svc_id_t service_id, ble_peripheral_char_id_t char_id);

static bool g_initialized = false;
static attr_server_config_t g_attr_server_config[] = ATTR_SERVER_CONFIG_INIT;
static attr_server_cb_t g_callbacks[ATTR_SERVER_MAX_CALLBACKS] = {NULL};


winsens_status_t attr_server_init(void)
{
    if (!g_initialized)
    {
        g_initialized = true;

        ble_peripheral_attr_subscribe(update_char_cb);
    }

    return WINSENS_OK;
}

winsens_status_t attr_server_subscribe(attr_server_cb_t callback)
{
    if (!g_initialized)
    {
        return WINSENS_NOT_INITIALIZED;
    }

    for (uint32_t i = 0; i < ATTR_SERVER_MAX_CALLBACKS; ++i)
    {
        if (NULL == g_callbacks[i])
        {
            g_callbacks[i] = callback;
            return WINSENS_OK;
        }
    }

    return WINSENS_NO_RESOURCES;
}

winsens_status_t attr_server_update(attr_server_attr_id_t attr_id, attr_server_value_t value)
{
    if (!g_initialized)
    {
        return WINSENS_NOT_INITIALIZED;
    }

    update_ble_server(attr_id, value);
    update_subscribers(attr_id, value);

    return WINSENS_OK;
}

void update_char_cb(ble_peripheral_update_t const *update_data)
{
    if (!g_initialized)
    {
        return;
    }

    attr_server_attr_id_t attr_id = get_attr_id(update_data->server_id, update_data->char_id);

    if (ATTR_SERVER_ID_NUMBER > attr_id)
    {
        attr_server_value_t value_data = {.value_len = update_data->value_len, .value = update_data->value};
        update_subscribers(attr_id, value_data);
    }
}

void update_subscribers(attr_server_attr_id_t attr_id, attr_server_value_t value)
{
    if (!g_initialized)
    {
        return;
    }

    for (uint32_t i = 0; i < ATTR_SERVER_MAX_CALLBACKS; ++i)
    {
        if (g_callbacks[i])
        {
            g_callbacks[i](attr_id, value);
        }
    }
}

void update_ble_server(attr_server_attr_id_t attr_id, attr_server_value_t value)
{
    if (!g_initialized)
    {
        return;
    }

    ble_peripheral_update(g_attr_server_config[attr_id].service_id, g_attr_server_config[attr_id].char_id, value.value_len, value.value);
}

attr_server_attr_id_t get_attr_id(ble_peripheral_svc_id_t service_id, ble_peripheral_char_id_t char_id)
{
    if (!g_initialized)
    {
        return ATTR_SERVER_ID_INVALID;
    }

    for (attr_server_attr_id_t i = 0; i < ATTRIBUTES_NUMBER; ++i)
    {
        if (char_id == g_attr_server_config[i].char_id && service_id == g_attr_server_config[i].service_id)
        {
            return i;
        }
    }

    return ATTR_SERVER_ID_INVALID;
}
