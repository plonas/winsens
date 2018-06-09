/*
 * ws_broker_stub.c
 *
 *  Created on: 6 cze 2018
 *      Author: Damian.Plonek
 */

#include "ws_broker_stub.h"

void WS_BrokerStubDeliver(
    const struct WS_Message_s *message);

WINSENS_Status_e WS_BrokerStubInit(
    WS_Broker_t *broker)
{
    broker->deliver = WS_BrokerStubDeliver;
    return WINSENS_OK;
}

void WS_BrokerStubDeinit(
    WS_Broker_t *broker)
{
    broker->deliver = NULL;
}

void WS_BrokerStubDeliver(
    const struct WS_Message_s *message)
{
    (void) message;
}
