/*
 * subscribers.c
 *
 *  Created on: 08.08.2021
 *      Author: Damian Plonek
 */


#include "subscribers.h"


winsens_status_t subscribers_init(subscribers_t* subscribers, winsens_event_handler_t* evt_handlers, uint32_t len)
{
    subscribers->len = len;
    subscribers->evt_handlers = evt_handlers;

    for (uint32_t i = 0; i < subscribers->len; ++i)
    {
        evt_handlers[i] = NULL;
    }

    return WINSENS_OK;
}

winsens_status_t subscribers_add(subscribers_t* subscribers, winsens_event_handler_t evt_handler)
{
    for (uint32_t i = 0; i < subscribers->len; ++i)
    {
        if (NULL == subscribers->evt_handlers[i])
        {
            subscribers->evt_handlers[i] = evt_handler;
            return WINSENS_OK;
        }
    }

    return WINSENS_NO_RESOURCES;


}

void subscribers_update(const subscribers_t* subscribers, winsens_event_t evt)
{
    for (uint32_t i = 0; i < subscribers->len; ++i)
    {
        if (subscribers->evt_handlers[i])
        {
            subscribers->evt_handlers[i](evt);
        }
    }
}

