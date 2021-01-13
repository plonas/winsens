/*
 * IServer.h
 *
 *  Created on: 6 cze 2018
 *      Author: Damian.Plonek
 */

#ifndef ISERVER_H_
#define ISERVER_H_

#include "winsens_types.h"
#include "IConfig.h"
#include "IWindowState.h"

struct WS_Server;

typedef enum
{
    WS_SERVER_EVENT_TYPE_THRESHOLD_UPDATE,
    WS_SERVER_EVENT_TYPE_ENABLED_UPDATE,
    WS_SERVER_EVENT_TYPE_APPLY,

} WS_ServerEventType_e;

typedef struct
{
    WS_ServerEventType_e eventType;
    union
    {
        uint16_t threshold;
        bool enabled;

    } value;

} WS_ServerEvent_t;

typedef void (*WS_ServerCallback_f)(IWindowId_t window, WS_ServerEvent_t event);

WINSENS_Status_e IServer_Init(
    const WS_Configuration_t *config);
void IServer_UpdateWindowState(
    IWindowId_t windowId,
    IWindowState_e state);
WINSENS_Status_e IServer_Subscribe(
    WS_ServerCallback_f callback);
void IServer_Unsubscribe(
    WS_ServerCallback_f callback);
void IServer_Reset(
    const WS_Configuration_t *config);
WINSENS_Status_e IServer_Disconnect(void);
WINSENS_Status_e IServer_DeletePeers(void);
void IServer_Deinit(void);


#endif /* ISERVER_H_ */
