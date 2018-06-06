/*
 * ws_broker_stub.h
 *
 *  Created on: 6 cze 2018
 *      Author: damian
 */

#ifndef WS_BROKER_STUB_H_
#define WS_BROKER_STUB_H_

#include "ws_broker.h"

WS_Broker_t * WS_BrokerStubCreate(void);

void WS_BrokerStubRemove(
    WS_Broker_t *broker);

#endif /* WS_BROKER_STUB_H_ */
