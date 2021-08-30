/*
 * window_state.h
 *
 *  Created on: 09.01.2021
 *      Author: Damian.Plonek
 */

#ifndef WINDOW_STATE_H_
#define WINDOW_STATE_H_

#include "winsens_types.h"


typedef enum
{
    WINDOW_STATE_UNKNOWN,
    WINDOW_STATE_OPEN,
    WINDOW_STATE_CLOSED

} window_state_type_t;

typedef enum
{
    WINDOW_STATE_EVT_OPEN = (WINSENS_IDS_MODULE_WINDOW_STATE << 16),
    WINDOW_STATE_EVT_CLOSED

} window_state_evt_t;

typedef uint8_t window_id_t;


winsens_status_t window_state_init(void);

winsens_status_t window_state_subscribe(winsens_event_handler_t callback);

winsens_status_t window_status_enable(window_id_t win_id);
void window_status_disable(window_id_t win_id);


#endif /* WINDOW_STATE_H_ */
