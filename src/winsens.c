/*
 * winsens.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "winsens.h"
#include "sensors/ws_distance.h"

static void WS_DistanceCallback(
    int16_t value);
static void WS_TimerCallback(
    nrf_timer_event_t eventType,
    void* context);

static const nrf_drv_timer_t ws_timer = NRF_DRV_TIMER_INSTANCE(0);

WINSENS_Status_e WINSENS_Init()
{
    WINSENS_Status_e status = WINSENS_ERROR;
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;

    // init a timer
    timer_cfg.frequency = NRF_TIMER_FREQ_31250Hz;
    ret_code_t err_code = nrf_drv_timer_init(&ws_timer, &timer_cfg, WS_TimerCallback);
    APP_ERROR_CHECK(err_code);
    nrf_drv_timer_extended_compare(&ws_timer, NRF_TIMER_CC_CHANNEL0, 31250UL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);

    // init a distance sensor
    status = WS_DistanceInit(WS_DistanceCallback, &ws_timer);
    return status;
}

void WINSENS_Deinit()
{
    nrf_drv_timer_uninit(&ws_timer);
    WS_DistanceDeinit();
}

WINSENS_Status_e WINSENS_Loop()
{
    WINSENS_Status_e status = WINSENS_ERROR;

    status = WS_DistanceStart();
    if (WINSENS_OK != status) return WINSENS_ERROR;

    WS_DistanceStop();
    return WINSENS_OK;
}

static void WS_DistanceCallback(
    int16_t value)
{
}

static void WS_TimerCallback(
    nrf_timer_event_t eventType,
    void* context)
{
}
