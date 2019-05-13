/*
 * ws_server_bt.h
 *
 *  Created on: 8 cze 2018
 *      Author: Damian.Plonek
 */

#ifndef WS_SERVER_BT_H_
#define WS_SERVER_BT_H_

#include "ws_server.h"
#include "ws_configuration.h"

WINSENS_Status_e WS_ServerBtInit(
    WS_Server_t *server,
    const WS_Configuration_t *config);

#endif /* WS_SERVER_BT_H_ */
