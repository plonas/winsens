/*
 * system.c
 *
 *  Created on: 27.12.2019
 *      Author: Damian Plonek
 */


#include "system.h"
#include "timer.h"
#include "button.h"
#define ILOG_MODULE_NAME SYS
#include "log.h"
#include "log_internal_nrf52.h"

#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"


#define APP_SOC_OBSERVER_PRIO                   1  /**< Applications' SoC observer priority. You shouldn't need to modify this value. */

#define SYSTEM_BUTTON_HOLD_DURATION             20 // it is 2000ms in 100ms unit


static void soc_evt_handler(uint32_t evt_id, void* p_context);
static void tmt_evt_handler(winsens_event_t evt);


static bool         g_initialized = false;
static timer_ws_t   g_timer;
static uint32_t     g_tmr_counter = 0;


LOG_REGISTER();


winsens_status_t system_init(void)
{
    winsens_status_t status = WINSENS_OK;

    if (false == g_initialized)
    {
        g_initialized = true;

        ret_code_t err_code = nrf_sdh_enable_request();
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

        // Register handlers for SoC events.
        NRF_SDH_SOC_OBSERVER(m_soc_observer, APP_SOC_OBSERVER_PRIO, soc_evt_handler, NULL);

        status = timer_init();
        LOG_ERROR_RETURN(status, status);

        status = timer_create(&g_timer, tmt_evt_handler, NULL);
        LOG_ERROR_RETURN(status, status);

        status = timer_start(&g_timer, 100, true);
        LOG_ERROR_RETURN(status, status);

        status = button_init();
        LOG_ERROR_RETURN(status, status);
    }

    return status;
}

uint32_t system_get_time(void)
{
    return g_tmr_counter;
}

static void soc_evt_handler(uint32_t evt_id, void* p_context)
{
    switch (evt_id)
    {
        default:
            // No implementation needed.
            break;
    }
}
static void tmt_evt_handler(winsens_event_t evt)
{
    ++g_tmr_counter;
}
