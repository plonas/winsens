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

typedef enum
{
    WS_BROKER_TOPIC_WINDOW_STATE,
    WS_BROKER_TOPIC_BATTERY,

} WS_BrokerTopic_e;

typedef void (*WS_BrokerDeliver_f)(const struct WS_Message_s *message);

typedef struct
{
    WS_Window_e windowId;
    WS_WindowState_e state;

} WS_BrokerWindowStateValue_t;

typedef union
{
    WS_BrokerWindowStateValue_t windowState;
    uint16_t batteryLevel;

} WS_BrokerMessageValue_t;

typedef struct WS_Message_s
{
    WS_BrokerTopic_e topic;
    WS_BrokerMessageValue_t value;

} WS_BrokerMessage_t;

typedef struct
{
    WS_BrokerDeliver_f deliver;

} WS_Broker_t;

#endif /* WS_BROKER_H_ */
