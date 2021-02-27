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


winsens_status_t IWinsens_Init(void)
{
    winsens_status_t status = WINSENS_ERROR;

    // init a window state
    status = window_state_init();
    LOG_ERROR_RETURN(status, status);

    status = hmi_init();
    LOG_ERROR_RETURN(status, status);

    return WINSENS_OK;
}

