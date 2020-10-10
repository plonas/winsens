/*
 * ws_button.c
 *
 *  Created on: 25.02.2020
 *      Author: Damian Plonek
 */

#include "ws_button.h"
#include "button_config.h"
#include "ws_system.h"
#define WS_LOG_MODULE_NAME "BUTN"
#include "ws_log.h"


#define WS_BUTTON_PINS_NUMBER               (sizeof(ws_buttonConfig) / sizeof(WS_DigitalInputPins_e))
#define WS_BUTTON_CALLBACK_INIT             { WS_DIGITAL_INPUT_PIN_INVALID, 0, NULL }

#define WS_BUTTON_PUSH_NORMAL_DURATION      1   // 100ms
#define WS_BUTTON_PUSH_LONG_DURATION        15  // 1500ms
#define WS_BUTTON_PUSH_VERY_LONG_DURATION   25  // 2500ms

typedef struct {
    WS_DigitalInputPins_e pin;
    uint32_t onTime;
    WS_EventHandler_f eventHandler;

} WS_ButtonPinCallback_t;

static void WS_DigitalInputCallback(
    WS_DigitalInputPins_e pin,
    bool on);

static WS_ButtonPinCallback_t ws_buttonCallbacks[WS_BUTTON_PINS_NUMBER];


WINSENS_Status_e WS_ButtonInit(void)
{
    uint32_t i;
    for (i = 0; i < WS_BUTTON_PINS_NUMBER; ++i)
    {
        ws_buttonCallbacks[i] = (WS_ButtonPinCallback_t) WS_BUTTON_CALLBACK_INIT;
    }

    return WINSENS_OK;
}

void WS_ButtonDeinit(void)
{
}

WINSENS_Status_e WS_ButtonRegisterCallback(
    WS_DigitalInputPins_e pin,
    WS_EventHandler_f eventHandler)
{
    uint32_t i;
    for (i = 0; i < WS_BUTTON_PINS_NUMBER; ++i)
    {
        if (ws_buttonConfig[i] == pin)
        {
            ws_buttonCallbacks[i].pin = pin;
            ws_buttonCallbacks[i].eventHandler = eventHandler;
            WINSENS_Status_e status = WS_DigitalInputRegisterCallback(pin, WS_DigitalInputCallback);
            if (WINSENS_OK != status)
            {
                ws_buttonCallbacks[i] = (WS_ButtonPinCallback_t) WS_BUTTON_CALLBACK_INIT;
                return status;
            }

            return WINSENS_OK;
        }
    }
    return WINSENS_NOT_FOUND;
}

void WS_ButtonUnregisterCallback(
    WS_DigitalInputPins_e pin)
{
    uint32_t i;
    for (i = 0; i < WS_BUTTON_PINS_NUMBER; ++i)
    {
        if (ws_buttonCallbacks[i].pin == pin)
        {
            ws_buttonCallbacks[i] = (WS_ButtonPinCallback_t) WS_BUTTON_CALLBACK_INIT;
            return;
        }
    }
}

static void WS_DigitalInputCallback(
    WS_DigitalInputPins_e pin,
    bool on)
{
    uint32_t i;
    WS_LOG_DEBUG("Button %u on %u\r\n", pin, on);
    for (i = 0; i < WS_BUTTON_PINS_NUMBER; ++i)
    {
        if (ws_buttonCallbacks[i].pin == pin &&
            ws_buttonCallbacks[i].eventHandler)
        {
            uint32_t duration = 0;

            if (on)
            {
                ws_buttonCallbacks[i].onTime = WS_SystemGetTime();
                return;
            }
            else
            {
                duration = WS_SystemGetTime() - ws_buttonCallbacks[i].onTime;
            }

            WS_ButtonEvent_e push = WS_BUTTON_EVENT_NORMAL;
            if (WS_BUTTON_PUSH_VERY_LONG_DURATION <= duration)
            {
                push = WS_BUTTON_EVENT_VERY_LONG;
            }
            else if (WS_BUTTON_PUSH_LONG_DURATION <= duration)
            {
                push = WS_BUTTON_EVENT_LONG;
            }
            else if (WS_BUTTON_PUSH_NORMAL_DURATION <= duration)
            {
                push = WS_BUTTON_EVENT_NORMAL;
            }
            else
            {
                // do nothing
                return;
            }

            WS_LOG_DEBUG("xxx button xxx\r\n");
            WS_Event_t e = { push, pin };
            ws_buttonCallbacks[i].eventHandler(e);
        }
    }
}
