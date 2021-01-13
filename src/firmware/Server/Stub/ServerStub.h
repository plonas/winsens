/*
 * ws_server_stub.h
 *
 *  Created on: 6 cze 2018
 *      Author: damian
 */

#ifndef SERVER_STUB_H_
#define SERVER_STUB_H_

#include "IServer.h"

WINSENS_Status_e ServerStub_Init(
    const WS_Configuration_t *config);
void ServerStub_UpdateWindowState(
    IWindowId_t windowId,
    IWindowState_e state);
WINSENS_Status_e ServerStub_Subscribe(
    WS_ServerCallback_f callback);
void ServerStub_Unsubscribe(
    WS_ServerCallback_f callback);
void ServerStub_Reset(
    const WS_Configuration_t *config);
WINSENS_Status_e ServerStub_Disconnect(void);
WINSENS_Status_e ServerStub_DeletePeers(void);
void ServerStub_Deinit(void);

#endif /* SERVER_STUB_H_ */
