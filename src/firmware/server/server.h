/*
 * server.h
 *
 *  Created on: 6 cze 2018
 *      Author: Damian.Plonek
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "winsens_types.h"
#include "config.h"
#include "window_state.h"


typedef enum
{
    SERVER_EVENT_TYPE_THRESHOLD_UPDATE,
    SERVER_EVENT_TYPE_ENABLED_UPDATE,
    SERVER_EVENT_TYPE_APPLY,

} server_event_type_t;

typedef struct
{
    server_event_type_t eventType;
    union
    {
        uint16_t    threshold;
        bool        enabled;

    } value;

} server_event_t;

typedef void (*server_callback_t)(window_id_t window, server_event_t event);

winsens_status_t server_init(void);
void server_update_window_state(
    window_id_t windowId,
    window_state_type_t state);
winsens_status_t server_subscribe(
    server_callback_t callback);
void server_unsubscribe(
    server_callback_t callback);
winsens_status_t server_disconnect(void);
winsens_status_t server_delete_peers(void);


#endif /* SERVER_H_ */
