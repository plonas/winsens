/*
 * winsens.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "winsens.h"
#include "ws_window_state.h"

#include "nrf_delay.h"
#define NRF_LOG_MODULE_NAME "WINSENS"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


static void WS_WindowStateCallback(
    WS_Window_e window,
    WS_WindowState_e state);
static void WS_ServerCallback(
    WS_Window_e window,
    WS_ServerEvent_t event);

WS_Server_t *ws_server = NULL;

WINSENS_Status_e WINSENS_Init(
    WS_Server_t *server)
{
    WINSENS_Status_e status = WINSENS_ERROR;

    NRF_LOG_INFO("WINSENS_Init\n");

    ws_server = server;

    // init a window state
    status = WS_WindowStateInit();
    WS_WindowStateSubscribe(WS_WINDOW_1, WS_WindowStateCallback);
    WS_WindowStateSubscribe(WS_WINDOW_2, WS_WindowStateCallback);
    server->subscribe(server, WS_WINDOW_1, WS_ServerCallback);
    server->subscribe(server, WS_WINDOW_2, WS_ServerCallback);

    return status;
}

void WINSENS_Deinit()
{
    NRF_LOG_INFO("WINSENS_Deinit\n");
    ws_server->unsubscribe(ws_server, WS_WINDOW_2, WS_ServerCallback);
    ws_server->unsubscribe(ws_server, WS_WINDOW_1, WS_ServerCallback);
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
    switch (event.eventType) {
        case WS_SERVER_EVENT_TYPE_THRESHOLD_UPDATE:
            WS_WindowStateConfigure(window, event.value.threshold); // todo handle return value
            break;
        case WS_SERVER_EVENT_TYPE_ENABLED_UPDATE:
            ws_server->enable(ws_server, window, event.value.enabled); // todo handle return value
            break;
        default:
            break;
    }
}
