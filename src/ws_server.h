/*
 * ws_server.h
 *
 *  Created on: 6 cze 2018
 *      Author: Damian.Plonek
 */

#ifndef WS_SERVER_H_
#define WS_SERVER_H_

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

typedef void (*WS_ServerUpdateWindowState_f)(struct WS_Server *server, IWindowId_t windowId, IWindowState_e state);
typedef WINSENS_Status_e (*WS_ServerSubscribe_f)(struct WS_Server *server, WS_ServerCallback_f callback);
typedef void (*WS_ServerUnsubscribe_f)(struct WS_Server *server, WS_ServerCallback_f callback);
typedef void (*WS_ServerReset_f)(struct WS_Server *server, const WS_Configuration_t *config);
typedef WINSENS_Status_e (*WS_ServerDisconnect_f)(void);
typedef WINSENS_Status_e (*WS_ServerDeletePeers_f)(void);
typedef void (*WS_ServerDeinit_f)(struct WS_Server *server);


typedef struct WS_Server
{
    WS_ServerUpdateWindowState_f updateWindowState;
    WS_ServerSubscribe_f subscribe;
    WS_ServerUnsubscribe_f unsubscribe;
    WS_ServerReset_f reset;
    WS_ServerDisconnect_f disconnect;
    WS_ServerDeletePeers_f deletePeers;
    WS_ServerDeinit_f deinit;

} WS_Server_t;

#endif /* WS_SERVER_H_ */
