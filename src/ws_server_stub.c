/*
 * ws_server_stub.c
 *
 *  Created on: 6 cze 2018
 *      Author: Damian.Plonek
 */

#include "ws_server_stub.h"

static void ws_ServerStubDeinit(
    WS_Server_t *server);

static void ws_ServerStubUpdateWindowState(
    WS_Server_t *server,
    WS_Window_e windowId,
    WS_WindowState_e state);


WINSENS_Status_e WS_ServerStubInit(
    WS_Server_t *server)
{
    server->updateWindowState = ws_ServerStubUpdateWindowState;
    server->deinit = ws_ServerStubDeinit;

    return WINSENS_OK;
}

void ws_ServerStubDeinit(
    WS_Server_t *server)
{
    server->updateWindowState = NULL;
    server->deinit = NULL;
}

static void ws_ServerStubUpdateWindowState(
    WS_Server_t *server,
    WS_Window_e windowId,
    WS_WindowState_e state)
{

}
