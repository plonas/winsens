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

typedef void (*WS_ServerUpdateWindowState_f)(struct WS_Server *server, WS_Window_e windowId, WS_WindowState_e state);
typedef void (*WS_ServerDeinit_f)(struct WS_Server *server);

typedef struct WS_Server
{
    WS_ServerUpdateWindowState_f updateWindowState;
    WS_ServerDeinit_f deinit;

} WS_Server_t;

#endif /* WS_SERVER_H_ */
