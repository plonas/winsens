/*
 * subscribers.h
 *
 *  Created on: 08.08.2021
 *      Author: Damian Plonek
 */

#ifndef SUBSCRIBERS_H_
#define SUBSCRIBERS_H_

#include "winsens_types.h"


typedef struct
{
    winsens_event_handler_t*    evt_handlers;
    uint32_t                    len;
} subscribers_t;


winsens_status_t subscribers_init(subscribers_t* subscribers, winsens_event_handler_t* evt_handlers, uint32_t len);

winsens_status_t subscribers_add(subscribers_t* subscribers, winsens_event_handler_t evt_handler);

void subscribers_update(const subscribers_t* subscribers, winsens_event_t evt);

#endif /* SUBSCRIBERS_H_ */
