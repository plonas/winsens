/*
 * ws_adc_adapter.c
 *
 *  Created on: 18.10.2018
 *      Author: Damian Plonek
 */

#include "hwal/ws_adc_adapter.h"
#include "hwal/ws_task_queue.h"
#include "hwal/ws_timer.h"
#include "utils/utils.h"
#define WS_LOG_MODULE_NAME ADCA
#include "ws_log.h"
#include "ws_log_nrf.h"

//#include "nrfx.h"
//#include "nrf_drv_saadc.h"
#include "nrfx_saadc.h"
#include "nrfx_ppi.h"
#include "nrfx_timer.h"

#include <string.h>


static const uint32_t ADC_CONFIG_INPUT_MAP[WS_ADC_ADAPTER_CHANNELS_NUMBER] = {
        NRF_SAADC_INPUT_AIN3,
        NRF_SAADC_INPUT_AIN6
};

typedef struct
{
    WS_AdcAdapterCallback_f callback;
    nrfx_saadc_channel_t nativeChannelConf;

} WS_AdcAdapterChannel_t;

static void WS_AdcAdapterIrqHandler(
    nrfx_saadc_evt_t const *event);
static void WS_TimerIrqHandler(
    nrf_timer_event_t eventType,
    void* context);
static void WS_AdcAdapterEventHandler(
    void *p_event_data,
    uint16_t event_size);


static nrf_ppi_channel_t ws_ppiChannelAdc;
static WS_AdcAdapterChannel_t ws_channels[WS_ADC_ADAPTER_CHANNELS_NUMBER];

static nrf_saadc_value_t ws_adc_buffer[WS_ADC_ADAPTER_CHANNELS_NUMBER];
static uint8_t ws_active_adc_channels_num = 0;

nrfx_timer_t ws_timer = NRFX_TIMER_INSTANCE(1);


WINSENS_Status_e WS_AdcAdapterInit(void)
{
    nrfx_err_t err_code = NRF_ERROR_INTERNAL;

    memset(ws_channels, 0, sizeof(WS_AdcAdapterChannel_t) * WS_ADC_ADAPTER_CHANNELS_NUMBER);
    ws_active_adc_channels_num = 0;

    // init a timer
    nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;
    err_code = nrfx_timer_init(&ws_timer, &timer_cfg, WS_TimerIrqHandler);
    WS_NRF_ERROR_CHECK(err_code, WINSENS_ERROR);
    nrfx_timer_extended_compare(&ws_timer, NRF_TIMER_CC_CHANNEL0, 31250UL, NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK, false);

    // init ADC
    err_code = nrfx_saadc_init(NRFX_SAADC_CONFIG_IRQ_PRIORITY);
    WS_NRF_ERROR_CHECK(err_code, WINSENS_ERROR);

    // init PPI
    err_code = nrfx_ppi_channel_alloc(&ws_ppiChannelAdc);
    WS_NRF_ERROR_CHECK(err_code, WINSENS_ERROR);
    err_code = nrfx_ppi_channel_assign(ws_ppiChannelAdc,
                                       nrfx_timer_event_address_get(&ws_timer, NRF_TIMER_EVENT_COMPARE0),
                                       nrf_saadc_task_address_get(NRF_SAADC_TASK_START));
    WS_NRF_ERROR_CHECK(err_code, WINSENS_ERROR);

    return WINSENS_OK;
}

void WS_AdcAdapterDeinit(void)
{
    nrfx_ppi_channel_free(ws_ppiChannelAdc);
    nrfx_saadc_uninit();
    nrfx_timer_uninit(&ws_timer);
}

WINSENS_Status_e WS_AdcAdapterEnableChannel(
    WS_AdcAdapterChannelId_e channelId,
    WS_AdcAdapterCallback_f callback)
{
    nrfx_err_t err_code;

    WS_ASSERT(WS_ADC_ADAPTER_CHANNELS_NUMBER > channelId);
    WS_ASSERT(callback);

    // Add channel to the list
    ws_channels[channelId].callback = callback;
    ws_channels[channelId].nativeChannelConf = (nrfx_saadc_channel_t) NRFX_SAADC_DEFAULT_CHANNEL_SE(ADC_CONFIG_INPUT_MAP[channelId], channelId);
//    ws_channels[channelId].nativeChannelConf.config.config.input = NRF_ADC_CONFIG_SCALING_INPUT_ONE_THIRD;

    // Increase the number of active channels
    ws_active_adc_channels_num++;

    // init the channel
    err_code = nrfx_saadc_channels_config(&ws_channels[channelId].nativeChannelConf, 1);
    WS_NRF_ERROR_CHECK(err_code, WINSENS_ERROR);
    WS_LOG_NRF_ERROR_CHECK(err_code);

    err_code = nrfx_saadc_simple_mode_set(channelId + 1, NRF_SAADC_RESOLUTION_8BIT, NRF_SAADC_OVERSAMPLE_DISABLED, WS_AdcAdapterIrqHandler); //todo bit mask
    WS_NRF_ERROR_CHECK(err_code, WINSENS_ERROR);

    return WINSENS_OK;
}

void WS_AdcAdapterDisableChannel(
    WS_AdcAdapterChannelId_e channelId)
{
    WS_ASSERT(WS_ADC_ADAPTER_CHANNELS_NUMBER > channelId);

    if (ws_active_adc_channels_num)
    {
        // Stop sampling
        nrfx_ppi_channel_disable(ws_ppiChannelAdc);
    }

    ws_channels[channelId].callback = NULL;
    ws_active_adc_channels_num--;

    if (ws_active_adc_channels_num)
    {
        nrfx_err_t err_code;
        // Start sampling
        err_code = nrfx_saadc_buffer_set(ws_adc_buffer, ws_active_adc_channels_num);
        WS_NRF_ERROR_CHECK(err_code, ;);
        err_code = nrfx_ppi_channel_enable(ws_ppiChannelAdc);
        WS_NRF_ERROR_CHECK(err_code, ;);
    }
}

WINSENS_Status_e WS_AdcAdapterStart(void)
{
    uint32_t err_code;

    // Start sampling
    err_code = nrfx_saadc_buffer_set(ws_adc_buffer, ws_active_adc_channels_num);
    WS_NRF_ERROR_CHECK(err_code, WINSENS_ERROR);
    err_code = nrfx_ppi_channel_enable(ws_ppiChannelAdc);
    WS_NRF_ERROR_CHECK(err_code, WINSENS_ERROR);

    return WINSENS_OK;
}

void WS_AdcAdapterStop(void)
{
    if (ws_active_adc_channels_num)
    {
        // Stop sampling
        nrfx_err_t err_code = nrfx_ppi_channel_disable(ws_ppiChannelAdc);
        WS_NRF_ERROR_CHECK(err_code, ;);
    }
}

static void WS_AdcAdapterIrqHandler(
    nrfx_saadc_evt_t const *event)
{
    if (NRFX_SAADC_EVT_DONE == event->type)
    {
        uint32_t i;

        if (ws_active_adc_channels_num != event->data.done.size)
        {
            WS_LOG_ERROR("Active channels #%u but got %u reads", ws_active_adc_channels_num, event->data.done.size);
            return;
        }

        for (i = 0; i < event->data.done.size; i++)
        {
            WINSENS_Status_e status = WINSENS_ERROR;
            WS_AdcAdapterEvent_t adcEvent = { i, (int16_t) event->data.done.p_buffer[i] };

            status = WS_TaskQueueAdd(&adcEvent, sizeof(adcEvent), WS_AdcAdapterEventHandler);
            if (WINSENS_OK != status)
            {
                WS_LOG_ERROR("WS_TaskQueueAdd failed");
            }
        }

        nrfx_err_t err_code = nrfx_saadc_buffer_set(ws_adc_buffer, ws_active_adc_channels_num);
        WS_LOG_NRF_ERROR_CHECK(err_code);
    }
}

static void WS_TimerIrqHandler(
    nrf_timer_event_t eventType,
    void* context)
{
}

static void WS_AdcAdapterEventHandler(
    void *p_event_data,
    uint16_t event_size)
{
    const WS_AdcAdapterEvent_t *adcEvent = p_event_data;
    UNUSED_PARAMETER(event_size);

    ASSERT(adcEvent);

    if (ws_channels[adcEvent->id].callback)
    {
        ws_channels[adcEvent->id].callback(adcEvent->id, adcEvent->value);
    }
    else
    {
        WS_LOG_ERROR("Missing channel's #%u callback", adcEvent->id);
    }
}
