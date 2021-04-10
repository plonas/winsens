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


static void timer_callback(
    WS_TimerId_t timerId);
static void soc_event_handler(
    uint32_t evt_id,
    void * p_context);

static bool                             g_initialized = false;
static WS_TimerId_t                     g_system_timer = 0;
static uint32_t                         g_time_counter = 0;


LOG_REGISTER();


winsens_status_t system_init(void)
{
    winsens_status_t status = WINSENS_OK;

    if (false == g_initialized)
    {
        g_initialized = true;

        uint32_t err_code;

        err_code = nrf_sdh_enable_request();
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

        // Register handlers for SoC events.
        NRF_SDH_SOC_OBSERVER(m_soc_observer, APP_SOC_OBSERVER_PRIO, soc_event_handler, NULL);

        status = ITimer_Init();
        LOG_ERROR_RETURN(status, status);

        status = digital_io_init();
        LOG_ERROR_RETURN(status, status);

        status = button_init();
        LOG_ERROR_RETURN(status, status);

        status = ITimer_SetTimer(100, true, timer_callback, &g_system_timer);
        LOG_ERROR_RETURN(status, status);
    }

    return status;
}

uint32_t system_get_time(void)
{
    return g_time_counter;
}

static void timer_callback(
    WS_TimerId_t timerId)
{
    ++g_time_counter;
}

static void soc_event_handler(uint32_t evt_id, void * p_context)
{
    switch (evt_id)
    {
        default:
            // No implementation needed.
            break;
    }
}

