/*
 * ServerStub.c
 *
 *  Created on: 6 cze 2018
 *      Author: Damian.Plonek
 */

#include "ServerStub.h"


#ifdef WINSENS_IF_SERVER_STUB
#define ServerStub_Init                   IServer_Init
#define ServerStub_UpdateWindowState      IServer_UpdateWindowState
#define ServerStub_Subscribe              IServer_Subscribe
#define ServerStub_Unsubscribe            IServer_Unsubscribe
#define ServerStub_Reset                  IServer_Reset
#define ServerStub_Disconnect             IServer_Disconnect
#define ServerStub_DeletePeers            IServer_DeletePeers
#define ServerStub_Deinit                 IServer_Deinit
#endif


WINSENS_Status_e ServerStub_Init(
    const WS_Configuration_t *config)
{
    return WINSENS_OK;
}

void ServerStub_UpdateWindowState(
    IWindowId_t windowId,
    IWindowState_e state)
{
}

WINSENS_Status_e ServerStub_Subscribe(
    WS_ServerCallback_f callback)
{
    return WINSENS_OK;
}

void ServerStub_Unsubscribe(
    WS_ServerCallback_f callback)
{
}

void ServerStub_Reset(
    const WS_Configuration_t *config)
{
}

WINSENS_Status_e ServerStub_Disconnect(void)
{
    return WINSENS_OK;
}

WINSENS_Status_e ServerStub_DeletePeers(void)
{
    return WINSENS_OK;
}

void ServerStub_Deinit(void)
{
}

