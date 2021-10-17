/*
 * winsens.h
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#ifndef IWINSENS_H_
#define IWINSENS_H_

#include "winsens_types.h"
#include "winsens_ids.h"


typedef enum
{
    WINSENS_EVT_IDLE = (WINSENS_IDS_MODULE_WINSENS << 16),
    WINSENS_EVT_WORKING,
} winsens_evt_t;


winsens_status_t winsens_init(void);

winsens_status_t winsens_subscribe(winsens_event_handler_t evt_handler);

#endif /* IWINSENS_H_ */
