/*
 * ws_server.h
 *
 *  Created on: 6 cze 2018
 *      Author: Damian.Plonek
 */

#ifndef WS_SERVER_H_
#define WS_SERVER_H_

#include "winsens_types.h"


struct WS_Server;

typedef void (*WS_ServerCallback_f)(WS_Window_e window, uint16_t value);

typedef void (*WS_ServerUpdateWindowState_f)(struct WS_Server *server, WS_Window_e windowId, WS_WindowState_e state);
typedef WINSENS_Status_e (*WS_ServerSubscribe_f)(struct WS_Server *server, WS_Window_e windowId, WS_ServerCallback_f callback);
typedef void (*WS_ServerUnsubscribe_f)(struct WS_Server *server, WS_Window_e windowId, WS_ServerCallback_f callback);
typedef void (*WS_ServerDeinit_f)(struct WS_Server *server);


typedef struct WS_Server
{
    WS_ServerUpdateWindowState_f updateWindowState;
    WS_ServerSubscribe_f subscribe;
    WS_ServerUnsubscribe_f unsubscribe;
    WS_ServerDeinit_f deinit;

} WS_Server_t;

#endif /* WS_SERVER_H_ */
