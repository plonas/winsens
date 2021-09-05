/*
 * button.c
 *
 *  Created on: 25.02.2020
 *      Author: Damian Plonek
 */

#include "button.h"
#include "button_cfg.h"
#include "digital_io_cfg.h"
#define ILOG_MODULE_NAME BUTN
#include "log.h"
#include "system.h"


#define BUTTONS_NUMBER                      (sizeof(g_buttons_config) / sizeof(digital_io_pin_t))
#define BUTTON_CALLBACK_INIT                { DIGITAL_INPUT_PIN_INVALID, 0, NULL }

#define BUTTON_PUSH_NORMAL_DURATION         1   // 100ms
#define BUTTON_PUSH_LONG_DURATION           15  // 1500ms
#define BUTTON_PUSH_VERY_LONG_DURATION      25  // 2500ms

typedef struct {
    digital_io_pin_t pin;
    uint32_t onTime;
    winsens_event_handler_t eventHandler;

} button_pin_callback_t;

static void digital_input_callback(
    digital_io_pin_t pin,
    bool on);

static bool g_initialized = false;
static const button_cfg_t g_buttons_config[] = BUTTONS_COOFIG_INIT;
static button_pin_callback_t g_button_callbacks[BUTTONS_NUMBER];


LOG_REGISTER();


winsens_status_t button_init(void)
{
    if (false == g_initialized)
    {
        g_initialized = true;

        uint32_t i;
        for (i = 0; i < BUTTONS_NUMBER; ++i)
        {
            g_button_callbacks[i] = (button_pin_callback_t) BUTTON_CALLBACK_INIT;
        }
    }

    return WINSENS_OK;
}

winsens_status_t button_register_callback(
    button_id_t btn,
    winsens_event_handler_t eventHandler)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    if (btn < BUTTONS_NUMBER)
    {
        winsens_status_t status = digital_io_register_callback(g_buttons_config[btn].pin, digital_input_callback);
        LOG_ERROR_RETURN(status, status);

        g_button_callbacks[btn].pin = g_buttons_config[btn].pin;
        g_button_callbacks[btn].eventHandler = eventHandler;

        return WINSENS_OK;
    }
    return WINSENS_NOT_FOUND;
}

void button_unregister_callback(
    button_id_t btn)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, );

    if (btn < BUTTONS_NUMBER)
    {
        g_button_callbacks[btn] = (button_pin_callback_t) BUTTON_CALLBACK_INIT;
    }
}

static void digital_input_callback(
    digital_io_pin_t pin,
    bool on)
{
    uint32_t i;
    for (i = 0; i < BUTTONS_NUMBER; ++i)
    {
        if (g_button_callbacks[i].pin == pin &&
            g_button_callbacks[i].eventHandler)
        {
            const bool isActive = g_buttons_config[i].active_low ? !on : on;
            uint32_t duration = 0;

            LOG_DEBUG("pin: %u, on: %u, onTime: %u", pin, isActive, g_button_callbacks[i].onTime);

            if (isActive)
            {
                g_button_callbacks[i].onTime = system_get_time();
                return;
            }
            else
            {
                duration = system_get_time() - g_button_callbacks[i].onTime;
            }

            button_event_t push = BUTTON_EVENT_NORMAL;
            if (BUTTON_PUSH_VERY_LONG_DURATION <= duration)
            {
                push = BUTTON_EVENT_VERY_LONG;
            }
            else if (BUTTON_PUSH_LONG_DURATION <= duration)
            {
                push = BUTTON_EVENT_LONG;
            }
            else if (BUTTON_PUSH_NORMAL_DURATION <= duration)
            {
                push = BUTTON_EVENT_NORMAL;
            }
            else
            {
                // do nothing
                return;
            }

            LOG_DEBUG("duration: %u ms", duration * 100);
            winsens_event_t e = { { push }, { pin } };
            g_button_callbacks[i].eventHandler(e);
        }
    }
}
