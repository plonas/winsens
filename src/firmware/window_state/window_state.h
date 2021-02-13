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

typedef uint8_t window_id_t;

typedef void (*window_state_callback_t)(window_id_t window, window_state_type_t state);


winsens_status_t window_state_init(void);

winsens_status_t window_state_subscribe(
    window_id_t windowsId,
    window_state_callback_t callback);
void window_state_unsubscribe(
    window_id_t windowsId,
    window_state_callback_t callback);

#endif /* WINDOW_STATE_H_ */
