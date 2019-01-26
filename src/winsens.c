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
    WS_BrokerTopic_e topic,
    WS_BrokerMessageValue_t value);

static void WS_WindowStateCallback(
    WS_Window_e window,
    WS_WindowState_e state);

static WS_Publisher_t ws_publisher;

WINSENS_Status_e WINSENS_Init(
    WS_Broker_t *broker)
{
    WINSENS_Status_e status = WINSENS_ERROR;

    NRF_LOG_INFO("WINSENS_Init\n");

    // init a window state
    status = WS_WindowStateInit(WS_WindowStateCallback);

    ws_publisher.broker = broker;

    return status;
}

void WINSENS_Deinit()
{
    NRF_LOG_INFO("WINSENS_Deinit\n");
    ws_publisher.broker = NULL;
    WS_WindowStateDeinit();
}

WINSENS_Status_e WINSENS_Loop()
{
    uint32_t counter = 0;

    NRF_LOG_INFO("WINSENS_Loop in\n");

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

    NRF_LOG_INFO("WINSENS_Loop out\n");
    return WINSENS_OK;
}

static WINSENS_Status_e WS_Publish(
    WS_BrokerTopic_e topic,
    WS_BrokerMessageValue_t value)
{
    WS_BrokerMessage_t msg = { topic, value };

    ws_publisher.broker->deliver(&msg);
    return WINSENS_OK;
}

static void WS_WindowStateCallback(
    WS_Window_e window,
    WS_WindowState_e state)
{
    WS_BrokerWindowStateValue_t wsValue = { window, state };
    WS_BrokerMessageValue_t value;
    value.windowState = wsValue;
    WS_Publish(WS_BROKER_TOPIC_WINDOW_STATE, value); //todo handle return value
}
