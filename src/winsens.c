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

    return status;
}

void WINSENS_Deinit()
{
    NRF_LOG_INFO("WINSENS_Deinit\n");
    WS_WindowStateUnsubscribe(WS_WINDOW_1, WS_WindowStateCallback);
    WS_WindowStateDeinit();
}

static void WS_WindowStateCallback(
    WS_Window_e window,
    WS_WindowState_e state)
{
    ws_server->updateWindowState(ws_server, window, state); // todo handle return value
}
