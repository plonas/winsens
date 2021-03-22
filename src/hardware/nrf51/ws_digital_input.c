/*
 * ws_switch.c
 *
 *  Created on: 17.01.2020
 *      Author: Damian Plonek
 */


#include "ws_digital_input.h"
#include "digital_input_config.h"
#define WS_LOG_MODULE_NAME " DIN"
#include "ws_task_queue.h"
#include "ws_log.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"


#define DIGITAL_IO_INPUT_PIN_CALLBACKS_INIT     { WS_DIGITAL_INPUT_PIN_INVALID, NULL, false }
#define DIGITAL_IO_INPUT_PINS_NUMBER            (sizeof(ws_digitalInputConfig) / sizeof(WS_DigitalInputPinCfg_t))


typedef struct
{
    digital_io_input_pin_t pin;
    digitalio_input_callback_t callback;
    bool status;

} WS_DigitalInputPinCallback_t;


static void WS_DigitalInputIrqHandler(
    nrf_drv_gpiote_pin_t pin,
    nrf_gpiote_polarity_t action);
static void WS_DigitalInputEventHandler(
    void *p_event_data,
    uint16_t event_size);

static nrf_gpio_pin_pull_t WS_ConvertPullUpDown(
    digital_io_pull_up_down_t pull);

static uint32_t ws_initCount = 0;
static WS_DigitalInputPinCallback_t ws_pinCallbacks[DIGITAL_IO_INPUT_PINS_NUMBER];


winsens_status_t WS_DigitalInputInit(void)
{
    ++ws_initCount;
    if (1 == ws_initCount)
    {
        uint8_t i = 0;

        for (i = 0; i < DIGITAL_IO_INPUT_PINS_NUMBER; ++i)
        {
            ws_pinCallbacks[i] = (WS_DigitalInputPinCallback_t) DIGITAL_IO_INPUT_PIN_CALLBACKS_INIT;
        }

        if (NRF_SUCCESS != nrf_drv_gpiote_init())
        {
            return WINSENS_ERROR;
        }

        for (i = 0; i < DIGITAL_IO_INPUT_PINS_NUMBER; ++i)
        {
            nrf_gpio_cfg_input(g_digital_io_input_config[i].pin, WS_ConvertPullUpDown(g_digital_io_input_config[i].pullUpDown));
        }
    }
    return WINSENS_OK;
}

void WS_DigitalInputDeinit(void)
{
    if (0 < ws_initCount)
    {
        --ws_initCount;
        if (0 == ws_initCount)
        {
            nrf_drv_gpiote_uninit();
        }
    }
}

winsens_status_t WS_DigitalInputRegisterCallback(
    digital_io_input_pin_t pin,
    digitalio_input_callback_t callback)
{
    uint8_t i = 0;

    for (i = 0; i < DIGITAL_IO_INPUT_PINS_NUMBER; ++i)
    {
        if (pin == g_digital_io_input_config[i].pin)
        {
            nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
            config.pull = WS_ConvertPullUpDown(g_digital_io_input_config[i].pullUpDown);
            ret_code_t ret = nrf_drv_gpiote_in_init(pin, &config, WS_DigitalInputIrqHandler);
            if (NRF_SUCCESS != ret)
            {
                ILOG_ERROR("nrf_drv_gpiote_in_init failed\r\n");
                return WINSENS_ERROR;
            }
            nrf_drv_gpiote_in_event_enable(pin, true);

            ws_pinCallbacks[i].pin = pin;
            ws_pinCallbacks[i].callback = callback;
            ws_pinCallbacks[i].status =  nrf_drv_gpiote_in_is_set(pin);

            return WINSENS_OK;
        }
    }

    return WINSENS_NOT_FOUND;
}

void WS_DigitalInputUnregisterCallback(
    digital_io_input_pin_t pin)
{
    uint8_t i = 0;

    for (i = 0; i < DIGITAL_IO_INPUT_PINS_NUMBER; ++i)
    {
        if (pin == ws_pinCallbacks[i].pin)
        {
            ws_pinCallbacks[i] = (WS_DigitalInputPinCallback_t) DIGITAL_IO_INPUT_PIN_CALLBACKS_INIT;

            nrf_drv_gpiote_in_event_disable(pin);
            nrf_drv_gpiote_in_uninit(pin);

            return;
        }
    }
}

static void WS_DigitalInputIrqHandler(
    nrf_drv_gpiote_pin_t pin,
    nrf_gpiote_polarity_t action)
{
    uint8_t i = 0;
    winsens_status_t status = WINSENS_ERROR;

    for (i = 0; i < DIGITAL_IO_INPUT_PINS_NUMBER; ++i)
    {
        if (pin == ws_pinCallbacks[i].pin)
        {
            status = WS_TaskQueueAdd(&i, sizeof(i), WS_DigitalInputEventHandler);
            if (WINSENS_OK != status)
            {
                ILOG_ERROR("WS_TaskQueueAdd failed\r\n");
            }
            break;
        }
    }
}

static void WS_DigitalInputEventHandler(
    void *p_event_data,
    uint16_t event_size)
{
    const uint8_t *pi =  p_event_data;
    const uint8_t i = *pi;
    UNUSED_PARAMETER(event_size);

    ws_pinCallbacks[i].status = !ws_pinCallbacks[i].status;
    ws_pinCallbacks[i].callback(ws_pinCallbacks[i].pin, ws_pinCallbacks[i].status);
}

static nrf_gpio_pin_pull_t WS_ConvertPullUpDown(
    digital_io_pull_up_down_t pull)
{
    switch (pull)
    {
    case WS_DIGITAL_INPUT_PULL_UP:
        return NRF_GPIO_PIN_PULLUP;
        break;
    case WS_DIGITAL_INPUT_PULL_DOWN:
        return NRF_GPIO_PIN_PULLDOWN;
        break;
    default:
        break;
    }

    return NRF_GPIO_PIN_NOPULL;
}
