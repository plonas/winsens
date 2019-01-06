/*
 * winsens.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "winsens.h"
#include "ws_window_state.h"
#include "ws_publisher.h"

#include "nrf_delay.h"
#define NRF_LOG_MODULE_NAME "WINSENS"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

static WINSENS_Status_e WS_Publish(
    WINSENS_Topic_e topic,
    int value);

static bool WS_IsWindowOpen(
    int16_t value);

static void WS_DistanceCallback(
    int16_t value);
static void WS_TimerCallback(
    nrf_timer_event_t eventType,
    void* context);

static const nrf_drv_timer_t ws_timer = NRF_DRV_TIMER_INSTANCE(1);

static WS_Publisher_t ws_publisher;

WINSENS_Status_e WINSENS_Init(
    WS_Broker_t *broker)
{
    WINSENS_Status_e status = WINSENS_ERROR;
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;

    NRF_LOG_INFO("WINSENS_Init\n");

    // init a timer
    timer_cfg.frequency = NRF_TIMER_FREQ_31250Hz;
    ret_code_t err_code = nrf_drv_timer_init(&ws_timer, &timer_cfg, WS_TimerCallback);
    APP_ERROR_CHECK(err_code);
//    nrf_drv_timer_compare(&ws_timer, NRF_TIMER_CC_CHANNEL0, 31250UL, false);
    nrf_drv_timer_extended_compare(&ws_timer, NRF_TIMER_CC_CHANNEL0, 31250UL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
//    nrf_drv_timer_extended_compare(&ws_timer, NRF_TIMER_CC_CHANNEL1, 51250UL, NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK, false);

    // init a window state
    status = WS_WindowStateInit();

    ws_publisher.broker = broker;

    return status;
}

void WINSENS_Deinit()
{
    NRF_LOG_INFO("WINSENS_Deinit\n");
    ws_publisher.broker = NULL;
    WS_WindowStateDeinit();
    nrf_drv_timer_uninit(&ws_timer);
}

WINSENS_Status_e WINSENS_Loop()
{
    WINSENS_Status_e status = WINSENS_ERROR;
    uint32_t counter = 0;

    NRF_LOG_INFO("WINSENS_Loop in\n");

    status = WS_DistanceStart();
    NRF_LOG_DEBUG("WS_DistanceStart returned %u\n", status);
    if (WINSENS_OK != status) return WINSENS_ERROR;

    nrf_drv_timer_enable(&ws_timer);
    while (true)
    {
        if (10 == counter)
        {
            NRF_LOG_FLUSH();
            counter = 0;
        }
        nrf_delay_ms(100);
        counter++;
    }

    nrf_drv_timer_disable(&ws_timer);
    WS_DistanceStop();
    NRF_LOG_INFO("WINSENS_Loop out\n");
    return WINSENS_OK;
}

static WINSENS_Status_e WS_Publish(
    WINSENS_Topic_e topic,
    int value)
{
    WS_Message_t msg = { topic, value };

    ws_publisher.broker->deliver(&msg);
    return WINSENS_OK;
}

static bool WS_IsWindowOpen(
    int16_t value)
{
    bool open = false;

    if (400 < value)
    {
        open = true;
    }

    NRF_LOG_DEBUG("WS_IsWindowOpen value %hu open %hu\n", value, open);
    return open;
}

static void WS_DistanceCallback(
    int16_t value)
{
    static uint_fast32_t c = 0;
    bool windowOpenState = false;

    NRF_LOG_DEBUG("WS_DistanceCallback [%u] value %hu\n", c++, value);

    windowOpenState = WS_IsWindowOpen(value);

    WS_Publish(WINSENS_TOPIC_WINDOW_STATE, windowOpenState); //todo handle return value
}

static void WS_TimerCallback(
    nrf_timer_event_t eventType,
    void* context)
{
    NRF_LOG_DEBUG("WS_TimerCallback\n");
}
