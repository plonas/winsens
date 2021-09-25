/*
 * window_state_observer.c
 *
 *  Created on: 02.09.20201
 *      Author: Damian.Plonek
 */

#include "ble_peripheral.h"
#include "ble_peripheral_cfg.h"
#include "window_state_observer.h"
#include "window_state_observer_cfg.h"
#include "window_state.h"
#define ILOG_MODULE_NAME win_state_obs
#include "log.h"


#define CHAR_WIN_MAP_LEN        ( sizeof(g_char_win_map) / sizeof(g_char_win_map[0]) )


typedef struct
{
    window_id_t                 win_id;
    ble_peripheral_char_id_t    threshold_char_id;
    ble_peripheral_char_id_t    state_char_id;
} window_state_observer_char_win_pair_t;


static void win_state_evt_handler(winsens_event_t evt);
static void ble_evt_handler(ble_peripheral_update_t const *update_data);
static void update_all_thresholds(void);
static bool get_th_win_id(ble_peripheral_char_id_t th_char_id, window_id_t *win_id);
static bool get_win_state_char_id(window_id_t win_id, ble_peripheral_char_id_t *state_char_id);


static const window_state_observer_char_win_pair_t g_char_win_map[] = WINDOW_STATE_OBSERVER_CFG_MAP_INIT;


LOG_REGISTER();


winsens_status_t window_state_observer_ble_init(void)
{
    winsens_status_t status = ble_peripheral_init();
    LOG_ERROR_RETURN(status, status);

    status = window_state_init();
    LOG_ERROR_RETURN(status, status);

    status = ble_peripheral_attr_subscribe(ble_evt_handler);
    LOG_ERROR_RETURN(status, status);

    status = window_state_subscribe(win_state_evt_handler);
    LOG_ERROR_RETURN(status, status);

    update_all_thresholds();

    return WINSENS_OK;
}

void win_state_evt_handler(winsens_event_t evt)
{
    ble_peripheral_char_id_t char_id;

    if (get_win_state_char_id(evt.data, &char_id))
    {
        uint8_t value = (uint8_t)window_state_get(evt.data);
        winsens_status_t status = ble_peripheral_update(BLE_PERIPERAL_SVC_WMS, char_id, sizeof(value), &value);
        LOG_ERROR_RETURN(status, ;);
    }
}

void ble_evt_handler(ble_peripheral_update_t const *update_data)
{
    if (BLE_PERIPERAL_SVC_CS == update_data->server_id)
    {
        window_id_t win_id;

        if (sizeof(window_state_threshold_t) == update_data->value_len &&
            get_th_win_id(update_data->char_id, &win_id))
        {
            window_state_threshold_t t = *(window_state_threshold_t*)update_data->value;
            winsens_status_t status = window_state_threshold_set(win_id, t);
            LOG_ERROR_RETURN(status, ;);
            LOG_DEBUG("window(%u) threshold update from ble %d", win_id, t);
        }
    }
}

void update_all_thresholds(void)
{
    const uint8_t windows_num = window_state_get_windows_number();
    window_state_threshold_t t;

    for(window_id_t w = 0; w < windows_num; ++w)
    {
        window_state_threshold_get(w, &t);
        winsens_status_t status = ble_peripheral_update(BLE_PERIPERAL_SVC_CS, BLE_PERIPERAL_CHAR_CS_THRESHOLD_WINDOW, sizeof(t), (uint8_t*)&t);
        LOG_ERROR_RETURN(status, ;);
    }
}

static bool get_th_win_id(ble_peripheral_char_id_t th_char_id, window_id_t *win_id)
{
    bool found = false;

    for (uint32_t i = 0; i < CHAR_WIN_MAP_LEN; ++i)
    {
        if (g_char_win_map[i].threshold_char_id == th_char_id)
        {
            *win_id = g_char_win_map[i].win_id;
            found = true;
            break;
        }
    }

    return found;
}

static bool get_win_state_char_id(window_id_t win_id, ble_peripheral_char_id_t *state_char_id)
{
    bool found = false;

    for (uint32_t i = 0; i < CHAR_WIN_MAP_LEN; ++i)
    {
        if (g_char_win_map[i].win_id == win_id)
        {
            *state_char_id = g_char_win_map[i].state_char_id;
            found = true;
            break;
        }
    }

    return found;
}