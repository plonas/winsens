/*
 * ws_system.c
 *
 *  Created on: 27.12.2019
 *      Author: Damian Plonek
 */


#include "hwal/ws_system.h"
#include "hwal/ws_digital_input.h"
#include "softdevice_handler.h"
#include "ws_configuration_write.h"
#include "winsens_config.h"
#define WS_LOG_MODULE_NAME " SYS"
#include "ws_log.h"

#include "fstorage.h"
#include "ble_advertising.h"


static void ws_sys_evt_dispatch(
    uint32_t sys_evt);
static void WS_DigitalInputCallback(
    WS_DigitalInputPin_t pin,
    bool on);


WINSENS_Status_e WS_SystemInit(void)
{
    uint32_t err_code = softdevice_sys_evt_handler_set(ws_sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    WS_DigitalInputPinCfg_t btnPinCfg = WS_DIGITAL_INPUT_PAIR_BTN_CFG;
    WS_DigitalInputInit();
    WS_DigitalInputSetPinConfig(WS_DIGITAL_INPUT_PAIR_BTN, btnPinCfg);
    WS_DigitalInputRegisterCallback(WS_DIGITAL_INPUT_PAIR_BTN, WS_DigitalInputCallback);

    return (NRF_SUCCESS == err_code) ? WINSENS_OK : WINSENS_ERROR;
}

void WS_SystemDeinit(void)
{
    WS_DigitalInputUnregisterCallback(WS_DIGITAL_INPUT_PAIR_BTN);
    WS_DigitalInputDeinit();
}

static void ws_sys_evt_dispatch(uint32_t sys_evt)
{
    // Dispatch the system event to the fstorage module, where it will be
    // dispatched to the Flash Data Storage (FDS) module.
    fs_sys_event_handler(sys_evt);

    // Dispatch to the Advertising module last, since it will check if there are any
    // pending flash operations in fstorage. Let fstorage process system events first,
    // so that it can report correctly to the Advertising module.
    ble_advertising_on_sys_evt(sys_evt);
}

static void WS_DigitalInputCallback(
    WS_DigitalInputPin_t pin,
    bool on)
{
    WS_LOG_DEBUG("xxx pin %u on %u\r\n", pin, on);
    const WS_Configuration_t *config = WS_ConfigurationGet();
    if (config->bonded)
    {
        WS_Configuration_t newConfig = *config;
        newConfig.bonded = false;
        memset(&newConfig.address, 0xFF, WS_CONFIGURATION_ADDR_LEN);
        WS_ConfigurationSet(&newConfig);
    }
}
