/*
 * ws_system.c
 *
 *  Created on: 27.12.2019
 *      Author: Damian Plonek
 */


#include "hwal/ws_system.h"
#include "hwal/ws_digital_input.h"
#include "hwal/ws_timer.h"
#include "hwal/ws_button.h"
#include "winsens_config.h"
#define WS_LOG_MODULE_NAME SYS
#include "ws_log.h"
#include "ws_log_nrf.h"

#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"


#define APP_SOC_OBSERVER_PRIO                   1  /**< Applications' SoC observer priority. You shouldn't need to modify this value. */

#define WS_SYSTEM_BUTTON_HOLD_DURATION          20 // it is 2000ms in 100ms unit


static void WS_TimerCallback(
    WS_TimerId_t timerId);
static void soc_evt_handler(
    uint32_t evt_id,
    void * p_context);

static WS_TimerId_t ws_systemTimer = 0;
static uint32_t ws_timeCounter = 0;

WINSENS_Status_e WS_SystemInit(void)
{
    uint32_t err_code;
    WINSENS_Status_e status = WINSENS_ERROR;

    err_code = nrf_sdh_enable_request();
    WS_LOG_NRF_ERROR_CHECK(err_code);


    // Register handlers for SoC events.
    NRF_SDH_SOC_OBSERVER(m_soc_observer, APP_SOC_OBSERVER_PRIO, soc_evt_handler, NULL);

    status = WS_TimerInit();
    WS_LOG_ERROR_CHECK(status);
    status = WS_DigitalInputInit();
    WS_LOG_ERROR_CHECK(status);
    status = WS_ButtonInit();
    WS_LOG_ERROR_CHECK(status);

    status = WS_TimerSetTimer(100, true, WS_TimerCallback, &ws_systemTimer);
    WS_LOG_ERROR_CHECK(status);

    return (NRF_SUCCESS == err_code) ? WINSENS_OK : WINSENS_ERROR;
}

void WS_SystemDeinit(void)
{
    WS_ButtonDeinit();
    WS_TimerCancel(ws_systemTimer);

    WS_DigitalInputDeinit();
    WS_TimerDeinit();
}

uint32_t WS_SystemGetTime(void)
{
    return ws_timeCounter;
}

static void WS_TimerCallback(
    WS_TimerId_t timerId)
{
    ++ws_timeCounter;
}

static void soc_evt_handler(uint32_t evt_id, void * p_context)
{
    switch (evt_id)
    {
        default:
            // No implementation needed.
            break;
    }
}

