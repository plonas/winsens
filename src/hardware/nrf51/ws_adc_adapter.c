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
#define WS_LOG_MODULE_NAME "ADCA"
#include "ws_log.h"

#include "nrf.h"
#include "nrf_drv_adc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"

#include <string.h>


const uint32_t ADC_CONFIG_INPUT_MAP[WS_ADC_ADAPTER_CHANNELS_NUMBER] = {
        NRF_ADC_CONFIG_INPUT_3,
        NRF_ADC_CONFIG_INPUT_6
};

typedef struct
{
    adc_callback_t callback;
    nrf_drv_adc_channel_t nativeChannelConf;

} WS_AdcAdapterChannel_t;

static void WS_AdcAdapterIrqHandler(
    const nrf_drv_adc_evt_t* event);
static void WS_AdcAdapterEventHandler(
    void *p_event_data,
    uint16_t event_size);


nrf_ppi_channel_t ws_ppiChannelAdc;
WS_AdcAdapterChannel_t ws_channels[WS_ADC_ADAPTER_CHANNELS_NUMBER];

static nrf_adc_value_t ws_adc_buffer[WS_ADC_ADAPTER_CHANNELS_NUMBER];
static uint8_t ws_active_adc_channels_num = 0;

static const nrf_drv_timer_t *ws_timer = NULL;


winsens_status_t WS_AdcAdapterInit(void)
{
    ret_code_t ret_code;
    uint32_t err_code = NRF_ERROR_INTERNAL;
    nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;
//    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;

    memset(ws_channels, 0, sizeof(WS_AdcAdapterChannel_t) * WS_ADC_ADAPTER_CHANNELS_NUMBER);
    ws_active_adc_channels_num = 0;

    // init a timer
    ws_timer = WS_TimerGetNativeTimer();
//    nrf_drv_timer_extended_compare(ws_timer, NRF_TIMER_CC_CHANNEL1, 31250UL, NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK, false);

    // init ADC
    ret_code = nrf_drv_adc_init(&config, WS_AdcAdapterIrqHandler);
    APP_ERROR_CHECK(ret_code);

    // init PPI
    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_alloc(&ws_ppiChannelAdc);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_ppi_channel_assign(ws_ppiChannelAdc,
                                          nrf_drv_timer_event_address_get(ws_timer, NRF_TIMER_EVENT_COMPARE0),
                                          nrf_drv_adc_start_task_get());
    APP_ERROR_CHECK(err_code);

    return WINSENS_OK;
}

void WS_AdcAdapterDeinit(void)
{
    nrf_drv_ppi_channel_free(ws_ppiChannelAdc);
    nrf_drv_ppi_uninit();

    nrf_drv_adc_uninit();
}

winsens_status_t WS_AdcAdapterEnableChannel(
    adc_channel_id_t channelId,
    adc_callback_t callback)
{
    UTILS_ASSERT(WS_ADC_ADAPTER_CHANNELS_NUMBER > channelId);
    UTILS_ASSERT(callback);

    // Add channel to the list
    ws_channels[channelId].callback = callback;
    ws_channels[channelId].nativeChannelConf = (nrf_drv_adc_channel_t) NRF_DRV_ADC_DEFAULT_CHANNEL(ADC_CONFIG_INPUT_MAP[channelId]);
    ws_channels[channelId].nativeChannelConf.config.config.input = NRF_ADC_CONFIG_SCALING_INPUT_ONE_THIRD;

    // Increase the number of active channels
    ws_active_adc_channels_num++;

    // Enable the channel
    nrf_drv_adc_channel_enable(&ws_channels[channelId].nativeChannelConf);

    return WINSENS_OK;
}

void WS_AdcAdapterDisableChannel(
    adc_channel_id_t channelId)
{
    UTILS_ASSERT(WS_ADC_ADAPTER_CHANNELS_NUMBER > channelId);

    if (ws_active_adc_channels_num)
    {
        // Stop sampling
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
    }
}

winsens_status_t WS_AdcAdapterStart(void)
{
    uint32_t err_code;

    // Start sampling
    err_code = nrf_drv_adc_buffer_convert(ws_adc_buffer, ws_active_adc_channels_num);
    WS_LOG_INFO("nrf_drv_adc_buffer_convert: %lu\r\n", err_code);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_ppi_channel_enable(ws_ppiChannelAdc);
    WS_LOG_INFO("nrf_drv_ppi_channel_enable: %lu\r\n", err_code);
    APP_ERROR_CHECK(err_code);

    return WINSENS_OK;
}

void WS_AdcAdapterStop(void)
{
    if (ws_active_adc_channels_num)
    {
        // Stop sampling
        nrf_drv_ppi_channel_disable(ws_ppiChannelAdc);
    }
}

static void WS_AdcAdapterIrqHandler(
    const nrf_drv_adc_evt_t* event)
{
    if (NRF_DRV_ADC_EVT_DONE == event->type)
    {
        uint32_t i;

        if (ws_active_adc_channels_num != event->data.done.size)
        {
            ILOG_ERROR("Active channels #%u but got %u reads\r\n", ws_active_adc_channels_num, event->data.done.size);
            return;
        }

        for (i = 0; i < event->data.done.size; i++)
        {
            winsens_status_t status = WINSENS_ERROR;
            adc_event_t adcEvent = { i, (int16_t) event->data.done.p_buffer[i] };

            status = WS_TaskQueueAdd(&adcEvent, sizeof(adcEvent), WS_AdcAdapterEventHandler);
            if (WINSENS_OK != status)
            {
                ILOG_ERROR("WS_TaskQueueAdd failed\r\n");
            }
        }

        APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(ws_adc_buffer, ws_active_adc_channels_num));
    }
}

static void WS_AdcAdapterEventHandler(
    void *p_event_data,
    uint16_t event_size)
{
    const adc_event_t *adcEvent = p_event_data;
    UNUSED_PARAMETER(event_size);

    UTILS_ASSERT(adcEvent);

    if (ws_channels[adcEvent->id].callback)
    {
        ws_channels[adcEvent->id].callback(adcEvent->id, adcEvent->value);
    }
    else
    {
        ILOG_ERROR("Missing channel's #%u callback\r\n", adcEvent->id);
    }
}
