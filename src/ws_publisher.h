/*
 * ws_publisher.h
 *
 *  Created on: 05.06.2018
 *      Author: Damian.Plonek
 */

#include "winsens_types.h"

#ifndef WS_PUBLISHER_H_
#define WS_PUBLISHER_H_

typedef enum
{
    WINSENS_EVENT_WINDOW_STATE,
    WINSENS_EVENT_BATTERY,

} WINSENS_Event_e;

WINSENS_Status_e WS_PublisherInit(void);

void WS_PublisherDeInit(void);

WINSENS_Status_e WS_PublisherPublish(
    WINSENS_Event_e event,
    int value);

#endif /* WS_PUBLISHER_H_ */
