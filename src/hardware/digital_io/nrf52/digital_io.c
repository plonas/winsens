/*
 * DigitalIoNrf52.c
 *
 *  Created on: 17.01.2020
 *      Author: Damian Plonek
 */


#include "digital_io.h"
#include "digital_io_cfg.h"
#define ILOG_MODULE_NAME DIN
#include "log.h"
#include "log_internal_nrf52.h"
#include "task_queue.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"


#define WS_DIGITAL_INPUT_PIN_CALLBACKS_INIT     { DIGITAL_INPUT_PIN_INVALID, NULL, false }
#define WS_DIGITAL_INPUT_PINS_NUMBER            (sizeof(DIGITAL_IO_INPUT_CONFIG) / sizeof(digital_io_input_pin_cfg_t))


typedef struct
{
    digital_io_input_pins_t     pin;
    digitalio_input_callback_t  callback;
    bool                        status;

} digital_io_input_pin_callback_t;


static void digital_io_input_isr(
    nrfx_gpiote_pin_t pin,
    nrf_gpiote_polarity_t action);
static void digital_io_input_event_handler(
    void *p_event_data,
    uint16_t event_size);

static nrf_gpio_pin_pull_t convert_pull_up_down(
    digital_io_pull_up_down_t pull);

static const digital_io_input_pin_cfg_t     DIGITAL_IO_INPUT_CONFIG[DIGITAL_IO_INPUT_NUMBER] = DIGITAL_IO_CFG_INPUT_INIT;
static uint32_t                             g_init_count = 0;
static digital_io_input_pin_callback_t      g_pin_callbacks[WS_DIGITAL_INPUT_PINS_NUMBER];


winsens_status_t digital_io_init(void)
{
    ++g_init_count;
    if (1 == g_init_count)
    {
        uint8_t i = 0;
        nrfx_err_t err_code;

        for (i = 0; i < WS_DIGITAL_INPUT_PINS_NUMBER; ++i)
        {
            g_pin_callbacks[i] = (digital_io_input_pin_callback_t) WS_DIGITAL_INPUT_PIN_CALLBACKS_INIT;
        }

        err_code = nrfx_gpiote_init();
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

        for (i = 0; i < WS_DIGITAL_INPUT_PINS_NUMBER; ++i)
        {
            nrf_gpio_cfg_input(DIGITAL_IO_INPUT_CONFIG[i].pin, convert_pull_up_down(DIGITAL_IO_INPUT_CONFIG[i].pullUpDown));
        }
    }
    return WINSENS_OK;
}

winsens_status_t digital_io_register_callback(
    digital_io_input_pins_t pin,
    digitalio_input_callback_t callback)
{
    uint8_t i = 0;

    for (i = 0; i < WS_DIGITAL_INPUT_PINS_NUMBER; ++i)
    {
        if (pin == DIGITAL_IO_INPUT_CONFIG[i].pin)
        {
            nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
            config.pull = convert_pull_up_down(DIGITAL_IO_INPUT_CONFIG[i].pullUpDown);
            ret_code_t ret = nrfx_gpiote_in_init((nrfx_gpiote_pin_t)pin, &config, digital_io_input_isr);
            LOG_NRF_ERROR_RETURN(ret, WINSENS_ERROR);

            nrfx_gpiote_in_event_enable(pin, true);

            g_pin_callbacks[i].pin = pin;
            g_pin_callbacks[i].callback = callback;
            g_pin_callbacks[i].status =  nrfx_gpiote_in_is_set(pin);

            return WINSENS_OK;
        }
    }

    return WINSENS_NOT_FOUND;
}

void digital_io_unregister_callback(
    digital_io_input_pins_t pin)
{
    uint8_t i = 0;

    for (i = 0; i < WS_DIGITAL_INPUT_PINS_NUMBER; ++i)
    {
        if (pin == g_pin_callbacks[i].pin)
        {
            g_pin_callbacks[i] = (digital_io_input_pin_callback_t) WS_DIGITAL_INPUT_PIN_CALLBACKS_INIT;

            nrfx_gpiote_in_event_disable(pin);
            nrfx_gpiote_in_uninit(pin);

            return;
        }
    }
}

static void digital_io_input_isr(
    nrfx_gpiote_pin_t pin,
    nrf_gpiote_polarity_t action)
{
    uint8_t i = 0;
    winsens_status_t status = WINSENS_ERROR;

    for (i = 0; i < WS_DIGITAL_INPUT_PINS_NUMBER; ++i)
    {
        if (pin == g_pin_callbacks[i].pin)
        {
            status = task_queue_add(&i, sizeof(i), digital_io_input_event_handler);
            LOG_IF_WARNING(status, "WS_TaskQueueAdd failed");
            break;
        }
    }
}

static void digital_io_input_event_handler(
    void *p_event_data,
    uint16_t event_size)
{
    const uint8_t *pi =  p_event_data;
    const uint8_t i = *pi;
    UNUSED_PARAMETER(event_size);

    g_pin_callbacks[i].status = !g_pin_callbacks[i].status;
    g_pin_callbacks[i].callback(g_pin_callbacks[i].pin, g_pin_callbacks[i].status);
}

static nrf_gpio_pin_pull_t convert_pull_up_down(
    digital_io_pull_up_down_t pull)
{
    switch (pull)
    {
    case DIGITAL_IO_INPUT_PULL_UP:
        return NRF_GPIO_PIN_PULLUP;
        break;
    case DIGITAL_IO_INPUT_PULL_DOWN:
        return NRF_GPIO_PIN_PULLDOWN;
        break;
    default:
        break;
    }

    return NRF_GPIO_PIN_NOPULL;
}
