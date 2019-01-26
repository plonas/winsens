/*
 * ws_adc_adapter.c
 *
 *  Created on: 18.10.2018
 *      Author: Damian Plonek
 */

#include "hwal/ws_adc_adapter.h"
#include "utils/utils.h"

#include "nrf.h"
#include "nrf_drv_adc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "app_error.h"
#define NRF_LOG_MODULE_NAME "ADC_ADAPTER"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include <string.h>


#define WS_ADC_MAX_CHANNELS          1

const uint32_t ADC_CONFIG_INPUT_MAP[WS_ADC_MAX_CHANNELS] = {
        NRF_ADC_CONFIG_INPUT_3
};

typedef struct
{
    WS_AdcAdapterCallback_f callback;
    nrf_drv_adc_channel_t nativeChannelConf;

} WS_AdcAdapterChannel_t;

static void WS_AdcAdapterEventHandler(
    const nrf_drv_adc_evt_t* event);
static void WS_TimerCallback(
    nrf_timer_event_t eventType,
    void* context);


nrf_ppi_channel_t ws_ppiChannelAdc;
WS_AdcAdapterChannel_t ws_channels[WS_ADC_MAX_CHANNELS];

static nrf_adc_value_t ws_adc_buffer[WS_ADC_MAX_CHANNELS];
static uint8_t ws_active_adc_channels_num = 0;

static const nrf_drv_timer_t ws_timer = NRF_DRV_TIMER_INSTANCE(1);


WINSENS_Status_e WS_AdcAdapterInit(void)
{
    ret_code_t ret_code;
    uint32_t err_code = NRF_ERROR_INTERNAL;
    nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;

    memset(ws_channels, 0, sizeof(WS_AdcAdapterChannel_t) * WS_ADC_MAX_CHANNELS);
    ws_active_adc_channels_num = 0;

    // init a timer
    timer_cfg.frequency = NRF_TIMER_FREQ_31250Hz;
    err_code = nrf_drv_timer_init(&ws_timer, &timer_cfg, WS_TimerCallback);
    APP_ERROR_CHECK(err_code);
    nrf_drv_timer_extended_compare(&ws_timer, NRF_TIMER_CC_CHANNEL0, 31250UL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);

    // init ADC
    ret_code = nrf_drv_adc_init(&config, WS_AdcAdapterEventHandler);
    APP_ERROR_CHECK(ret_code);

    // init PPI
    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_alloc(&ws_ppiChannelAdc);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_ppi_channel_assign(ws_ppiChannelAdc,
                                          nrf_drv_timer_event_address_get(&ws_timer, NRF_TIMER_EVENT_COMPARE0),
                                          nrf_drv_adc_start_task_get());
    APP_ERROR_CHECK(err_code);

    return WINSENS_OK;
}

void WS_AdcAdapterDeinit(void)
{
    nrf_drv_ppi_channel_free(ws_ppiChannelAdc);
    nrf_drv_ppi_uninit();

    nrf_drv_adc_uninit();

    nrf_drv_timer_uninit(&ws_timer);
}

WINSENS_Status_e WS_AdcAdapterEnableChannel(
    WS_AdcAdapterChannelId_e channelId,
    WS_AdcAdapterCallback_f callback)
{
    WS_ASSERT(WS_ADC_MAX_CHANNELS > channelId);
    WS_ASSERT(callback);

    if (ws_active_adc_channels_num)
    {
        // Stop sampling
        nrf_drv_timer_disable(&ws_timer);
        nrf_drv_ppi_channel_disable(ws_ppiChannelAdc);
    }

    // Add channel to the list
    ws_channels[channelId].callback = callback;
    ws_channels[channelId].nativeChannelConf = (nrf_drv_adc_channel_t) NRF_DRV_ADC_DEFAULT_CHANNEL(ADC_CONFIG_INPUT_MAP[channelId]);
    ws_channels[channelId].nativeChannelConf.config.config.input = NRF_ADC_CONFIG_SCALING_INPUT_ONE_THIRD;

    // Increase the number of active channels
    ws_active_adc_channels_num++;

    // Enable the channel
    nrf_drv_adc_channel_enable(&ws_channels[channelId].nativeChannelConf);

    // Start sampling
    APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(ws_adc_buffer, ws_active_adc_channels_num));
    APP_ERROR_CHECK(nrf_drv_ppi_channel_enable(ws_ppiChannelAdc));
    nrf_drv_timer_enable(&ws_timer);

    return WINSENS_OK;
}

void WS_AdcAdapterDisableChannel(
    WS_AdcAdapterChannelId_e channelId)
{
    WS_ASSERT(WS_ADC_MAX_CHANNELS > channelId);

    if (ws_active_adc_channels_num)
    {
        // Stop sampling
        nrf_drv_timer_disable(&ws_timer);
        nrf_drv_ppi_channel_disable(ws_ppiChannelAdc);
    }

    ws_channels[channelId].callback = NULL;
    ws_active_adc_channels_num--;

    // Disable the channel
    nrf_drv_adc_channel_disable(&ws_channels[channelId].nativeChannelConf);

    if (ws_active_adc_channels_num)
    {
        // Start sampling
        APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(ws_adc_buffer, ws_active_adc_channels_num));
        APP_ERROR_CHECK(nrf_drv_ppi_channel_enable(ws_ppiChannelAdc));
        nrf_drv_timer_enable(&ws_timer);
    }
}

static void WS_AdcAdapterEventHandler(
    const nrf_drv_adc_evt_t* event)
{
    if (NRF_DRV_ADC_EVT_DONE == event->type)
    {
        uint32_t i;

        if (ws_active_adc_channels_num != event->data.done.size)
        {
            NRF_LOG_ERROR("Active channels #%u but got %u reads\r\n", ws_active_adc_channels_num, event->data.done.size);
            return;
        }

        for (i = 0; i < event->data.done.size; i++)
        {
            if (ws_channels[i].callback)
            {
                ws_channels[i].callback(i, event->data.done.p_buffer[i]);
            }
            else
            {
                NRF_LOG_ERROR("Missing channel's #%u callback\r\n", i);
            }
        }

        APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(ws_adc_buffer, ws_active_adc_channels_num));
    }
}

static void WS_TimerCallback(
    nrf_timer_event_t eventType,
    void* context)
{
    NRF_LOG_DEBUG("WS_TimerCallback\n");
}