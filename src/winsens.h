/*
 * winsens.h
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#ifndef WINSENS_H_
#define WINSENS_H_

#include "winsens_types.h"
#include "ws_publisher.h" //to show it is a publisher

WINSENS_Status_e WINSENS_Init(
    WS_Broker_t *broker);

void WINSENS_Deinit();

WINSENS_Status_e WINSENS_Loop();

#endif /* WINSENS_H_ */
