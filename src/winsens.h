/*
 * winsens.h
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#ifndef WINSENS_H_
#define WINSENS_H_

#include "winsens_types.h"
#include "ws_server.h"
#include "ws_configuration.h"
#include "winsens_config.h"

WINSENS_Status_e WINSENS_Init(
    WS_Server_t *server,
    const WS_Configuration_t *config);

void WINSENS_Deinit();

#endif /* WINSENS_H_ */
