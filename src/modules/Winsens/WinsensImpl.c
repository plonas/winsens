/*
 * WinsensImpl.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "IWinsens.h"
#include "WinsensImpl.h"
#include "IWindowState.h"
#include "IConfig.h"
#define WS_LOG_MODULE_NAME WNSN
#include "ws_log.h"

#ifdef WINSENS_IF_WINSENS_IMPL
#define WinsensImpl_Init            IWinsens_Init
#define WinsensImpl_Deinit          IWinsens_Deinit
#endif

static void WS_WindowStateCallback(
    IWindowId_t window,
    IWindowState_e state);
static void WS_ServerCallback(
    IWindowId_t window,
    WS_ServerEvent_t event);

WS_Server_t *ws_server = NULL;
const WS_Configuration_t *ws_config = NULL;


WINSENS_Status_e WinsensImpl_Init(
    WS_Server_t *server,
    const WS_Configuration_t *config)
{
    WINSENS_Status_e status = WINSENS_ERROR;

    ws_server = server;
    ws_config = config;

    // init a window state
    status = IWindowStateInit();
    WS_ERROR_CHECK(status, status);

    if (config->windowEnabled[IWINDOW_STATE_CFG_WINDOW_1])
    {
        IWindowStateSubscribe(IWINDOW_STATE_CFG_WINDOW_1, WS_WindowStateCallback);
    }
    if (config->windowEnabled[IWINDOW_STATE_CFG_WINDOW_2])
    {
        IWindowStateSubscribe(IWINDOW_STATE_CFG_WINDOW_2, WS_WindowStateCallback);
    }

    server->subscribe(server, WS_ServerCallback);

    return WINSENS_OK;
}

void WinsensImpl_Deinit()
{
    ws_server->unsubscribe(ws_server, WS_ServerCallback);
}

static void WS_WindowStateCallback(
    IWindowId_t window,
    IWindowState_e state)
{
    ws_server->updateWindowState(ws_server, window, state);
}

static void WS_ServerCallback(
    IWindowId_t window,
    WS_ServerEvent_t event)
{
    switch (event.eventType) {
        case WS_SERVER_EVENT_TYPE_THRESHOLD_UPDATE:
        {
            break;
        }

        case WS_SERVER_EVENT_TYPE_ENABLED_UPDATE:
        {
            WS_Configuration_t newConfig = *ws_config;

            if (ws_config->windowEnabled[window] != event.value.enabled)
            {
                newConfig.windowEnabled[window] = event.value.enabled;
                IConfig_Set(&newConfig);

                if (newConfig.windowEnabled[window])
                {
                    WINSENS_Status_e status = IWindowStateSubscribe(window, WS_WindowStateCallback);
                    WS_LOG_WARNING_CHECK(status);
                }
                else
                {
                    IWindowStateUnsubscribe(window, WS_WindowStateCallback);
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

