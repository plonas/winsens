/*
 * ws_broker.h
 *
 *  Created on: 6 cze 2018
 *      Author: Damian.Plonek
 */

#ifndef WS_BROKER_H_
#define WS_BROKER_H_

#include "winsens_types.h"

struct WS_Message_s;

typedef void (*WS_BrokerDeliver_f)(const struct WS_Message_s *message);

typedef struct WS_Message_s
{
    WINSENS_Topic_e topic;
    int value;

} WS_Message_t;

typedef struct
{
    WS_BrokerDeliver_f deliver;

} WS_Broker_t;

#endif /* WS_BROKER_H_ */
