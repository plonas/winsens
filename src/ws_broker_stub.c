/*
 * ws_broker_stub.c
 *
 *  Created on: 6 cze 2018
 *      Author: Damian.Plonek
 */

#include "ws_broker_stub.h"

void WS_BrokerStubDeliver(
    const struct WS_Message_s *message);

WS_Broker_t brokerStub = { WS_BrokerStubDeliver };

WS_Broker_t * WS_BroketStubCreate(void)
{
    return &brokerStub;
}

void WS_BroketStubRemove(
    WS_Broker_t *broker)
{
    (void) broker;
}

void WS_BrokerStubDeliver(
    const struct WS_Message_s *message)
{
    (void) message;
}
