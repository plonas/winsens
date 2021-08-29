/*
 * WinsensImpl.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "hmi.h"
#include "winsens.h"
#include "window_state.h"
#include "window_state_cfg.h"
#include "config.h"
#define ILOG_MODULE_NAME WNSN
#include "log.h"
#include "acc.h"


LOG_REGISTER();


void window_state_callback(window_id_t window, window_state_type_t state);


winsens_status_t winsens_init(void)
{
    winsens_status_t status = WINSENS_ERROR;

    // init a window state
    status = window_state_init();
    LOG_ERROR_RETURN(status, status);

    status = hmi_init();
    LOG_ERROR_RETURN(status, status);

    acc_init();

    window_state_subscribe(WINDOW_STATE_CFG_WINDOW_LEFT, window_state_callback);

    return WINSENS_OK;
}

void window_state_callback(window_id_t window, window_state_type_t state)
{
    LOG_DEBUG("window %u state %u", window, state);
}
