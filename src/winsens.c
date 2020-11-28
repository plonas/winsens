/*
 * winsens.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "winsens.h"
#include "ws_window_state.h"
#include "ws_configuration_write.h"
#define WS_LOG_MODULE_NAME WNSN
#include "ws_log.h"


static void WS_WindowStateCallback(
    WS_Window_e window,
    WS_WindowState_e state);
static void WS_ServerCallback(
    WS_Window_e window,
    WS_ServerEvent_t event);

WS_Server_t *ws_server = NULL;
const WS_Configuration_t *ws_config = NULL;


WINSENS_Status_e WINSENS_Init(
    WS_Server_t *server,
    const WS_Configuration_t *config)
{
    WINSENS_Status_e status = WINSENS_ERROR;

    WS_LOG_INFO("WINSENS_Init\r\n");

    ws_server = server;
    ws_config = config;

    // init a window state
    status = WS_WindowStateInit();
    if (WINSENS_OK != status)
    {
        return status;
    }

    if (config->windowEnabled[WS_WINDOW_1])
    {
        WS_WindowStateSubscribe(WS_WINDOW_1, WS_WindowStateCallback);
    }
    if (config->windowEnabled[WS_WINDOW_2])
    {
        WS_WindowStateSubscribe(WS_WINDOW_2, WS_WindowStateCallback);
    }

    server->subscribe(server, WS_ServerCallback);

    return WINSENS_OK;
}

void WINSENS_Deinit()
{
    WS_LOG_INFO("WINSENS_Deinit\r\n");
    ws_server->unsubscribe(ws_server, WS_ServerCallback);
    WS_WindowStateUnsubscribe(WS_WINDOW_2, WS_WindowStateCallback);
    WS_WindowStateUnsubscribe(WS_WINDOW_1, WS_WindowStateCallback);
    WS_WindowStateDeinit();
}

static void WS_WindowStateCallback(
    WS_Window_e window,
    WS_WindowState_e state)
{
    ws_server->updateWindowState(ws_server, window, state); // todo handle return value
}

static void WS_ServerCallback(
    WS_Window_e window,
    WS_ServerEvent_t event)
{
    WS_LOG_INFO("WS_ServerCallback event: %d\r\n", event.eventType);

    switch (event.eventType) {
        case WS_SERVER_EVENT_TYPE_THRESHOLD_UPDATE:
        {
            WS_Configuration_t newConfig = *ws_config;
            newConfig.windowThreshold[window] = event.value.threshold;
            WS_ConfigurationSet(&newConfig);

            WS_WindowStateConfigure(window, event.value.threshold); // todo handle return value
            break;
        }

        case WS_SERVER_EVENT_TYPE_ENABLED_UPDATE:
        {
            WS_Configuration_t newConfig = *ws_config;

            if (ws_config->windowEnabled[window] != event.value.enabled)
            {
                newConfig.windowEnabled[window] = event.value.enabled;
                WS_ConfigurationSet(&newConfig);

                if (newConfig.windowEnabled[window])
                {
                    WS_WindowStateSubscribe(window, WS_WindowStateCallback);
                }
                else
                {
                    WS_WindowStateUnsubscribe(window, WS_WindowStateCallback);
                }
            }
            else
            {
            }

            break;
        }

        case WS_SERVER_EVENT_TYPE_APPLY:
            ws_server->reset(ws_server, ws_config);
            break;

        default:
            break;
    }
}

