/*
 * ServerBt.h
 *
 *  Created on: 8 cze 2018
 *      Author: Damian.Plonek
 */

#ifndef SERVER_BT_H_
#define SERVER_BT_H_

#include "IServer.h"
#include "IConfig.h"

WINSENS_Status_e ServerBt_Init(
    const WS_Configuration_t *config);
void ServerBt_UpdateWindowState(
    IWindowId_t windowId,
    IWindowState_e state);
WINSENS_Status_e ServerBt_Subscribe(
    WS_ServerCallback_f callback);
void ServerBt_Unsubscribe(
    WS_ServerCallback_f callback);
void ServerBt_Reset(
    const WS_Configuration_t *config);
WINSENS_Status_e ServerBt_Disconnect(void);
WINSENS_Status_e ServerBt_DeletePeers(void);
void ServerBt_Deinit(void);

#endif /* SERVER_BT_H_ */
