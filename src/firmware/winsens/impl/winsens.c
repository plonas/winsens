/*
 * WinsensImpl.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "winsens.h"
#include "window_state.h"
#include "window_state_cfg.h"
#include "config.h"
#include "server.h"
#define ILOG_MODULE_NAME WNSN
#include "log.h"


static void WS_WindowStateCallback(
    window_id_t window,
    window_state_type_t state);
static void WS_ServerCallback(
    window_id_t window,
    server_event_t event);

winsens_status_t IWinsens_Init(void)
{
    winsens_status_t status = WINSENS_ERROR;

    // init a window state
    status = window_state_init();
    LOG_ERROR_RETURN(status, status);

//    if (config->windowEnabled[WINDOW_STATE_CFG_WINDOW_LEFT])
    {
        window_state_subscribe(WINDOW_STATE_CFG_WINDOW_LEFT, WS_WindowStateCallback);
    }
//    if (config->windowEnabled[WINDOW_STATE_CFG_WINDOW_RIGHT])
    {
        window_state_subscribe(WINDOW_STATE_CFG_WINDOW_RIGHT, WS_WindowStateCallback);
    }

    server_subscribe(WS_ServerCallback);

    return WINSENS_OK;
}

static void WS_WindowStateCallback(
    window_id_t window,
    window_state_type_t state)
{
    server_update_window_state(window, state);
}

static void WS_ServerCallback(
    window_id_t window,
    server_event_t event)
{
    switch (event.eventType) {
        case SERVER_EVENT_TYPE_THRESHOLD_UPDATE:
        {
            break;
        }

        case SERVER_EVENT_TYPE_ENABLED_UPDATE:
        {
//            if (ws_config->windowEnabled[window] != event.value.enabled)
//            {
//                newConfig.windowEnabled[window] = event.value.enabled;
//                config_set(&newConfig);
//
//                if (newConfig.windowEnabled[window])
//                {
//                    winsens_status_t status = window_state_subscribe(window, WS_WindowStateCallback);
//                    LOG_WARNING_CHECK(status);
//                }
//                else
//                {
//                    window_state_unsubscribe(window, WS_WindowStateCallback);
//                }
//            }
//            else
//            {
//            }

            break;
        }

        case SERVER_EVENT_TYPE_APPLY:
//            server_reset(ws_config);
            break;

        default:
            break;
    }
}

