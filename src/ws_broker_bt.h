/*
 * ws_broker_bt.h
 *
 *  Created on: 8 cze 2018
 *      Author: Damian.Plonek
 */

#ifndef WS_BROKER_BT_H_
#define WS_BROKER_BT_H_

#include "ws_broker.h"

WINSENS_Status_e WS_BrokerBtInit(
    WS_Broker_t *broker);

void WS_BrokerBtDeinit(
    WS_Broker_t *broker);

#endif /* WS_BROKER_BT_H_ */
