/*
 * ws_distance.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "ws_distance.h"

#include "nrf.h"
#include "nrf_drv_adc.h"
#include "nrf_drv_ppi.h"
#include "app_error.h"


#define ADC_BUFFER_LEN          1

static void WS_AdcEventHandler(
    const nrf_drv_adc_evt_t* event);

static nrf_adc_value_t       ws_adc_buffer[ADC_BUFFER_LEN];
static nrf_drv_adc_channel_t ws_channel_config = NRF_DRV_ADC_DEFAULT_CHANNEL(NRF_ADC_CONFIG_INPUT_2);
static WS_DistanceCallback_f ws_callback;
static nrf_ppi_channel_t ws_ppiChannel;


WINSENS_Status_e WS_DistanceInit(
    WS_DistanceCallback_f callback,
    const nrf_drv_timer_t* timer)
{
    ret_code_t ret_code;
    uint32_t err_code = NRF_ERROR_INTERNAL;
    nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;

    if (NULL == callback) return WINSENS_ERROR;

    // init ADC
    ret_code = nrf_drv_adc_init(&config, WS_AdcEventHandler);
    APP_ERROR_CHECK(ret_code);

    // init PPI
    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_ppi_channel_alloc(&ws_ppiChannel);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_ppi_channel_assign(ws_ppiChannel,
                                          nrf_drv_timer_event_address_get(timer, NRF_TIMER_EVENT_COMPARE0),
                                          nrf_drv_adc_start_task_get());
    APP_ERROR_CHECK(err_code);

    ws_callback = callback;

    return WINSENS_OK;
}

void WS_DistanceDeinit()
{
    nrf_drv_ppi_channel_free(ws_ppiChannel);
    nrf_drv_ppi_uninit();

    nrf_drv_adc_uninit();
}

WINSENS_Status_e WS_DistanceStart()
{
    nrf_drv_adc_channel_enable(&ws_channel_config);
    APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(ws_adc_buffer, ADC_BUFFER_LEN));
    APP_ERROR_CHECK(nrf_drv_ppi_channel_enable(ws_ppiChannel));

    return WINSENS_OK;
}

void WS_DistanceStop()
{
    APP_ERROR_CHECK(nrf_drv_ppi_channel_disable(ws_ppiChannel));
    nrf_drv_adc_channel_disable(&ws_channel_config);
}

static void WS_AdcEventHandler(
    const nrf_drv_adc_evt_t* event)
{
    if (NRF_DRV_ADC_EVT_DONE == event->type)
    {
        if (0 < event->data.done.size)
            ws_callback(event->data.done.p_buffer[0]);

        APP_ERROR_CHECK(nrf_drv_adc_buffer_convert(ws_adc_buffer, ADC_BUFFER_LEN));
    }
}
