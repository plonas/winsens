/*
 * server.c
 *
 *  Created on: 6 cze 2018
 *      Author: Damian.Plonek
 */

#include "server.h"


winsens_status_t server_init(
    const config_t *config)
{
    return WINSENS_OK;
}

void server_update_window_state(
    window_id_t windowId,
    window_state_type_t state)
{
}

winsens_status_t server_subscribe(
    server_callback_t callback)
{
    return WINSENS_OK;
}

void server_unsubscribe(
        server_callback_t callback)
{
}

void server_reset(
    const config_t *config)
{
}

winsens_status_t server_disconnect(void)
{
    return WINSENS_OK;
}

winsens_status_t server_delete_peers(void)
{
    return WINSENS_OK;
}

void server_deinit(void)
{
}

