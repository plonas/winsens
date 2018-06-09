/*
 * ws_broker_stub.h
 *
 *  Created on: 6 cze 2018
 *      Author: damian
 */

#ifndef WS_BROKER_STUB_H_
#define WS_BROKER_STUB_H_

#include "ws_broker.h"

WINSENS_Status_e WS_BrokerStubInit(
    WS_Broker_t *broker);

void WS_BrokerStubDeinit(
    WS_Broker_t *broker);

#endif /* WS_BROKER_STUB_H_ */
