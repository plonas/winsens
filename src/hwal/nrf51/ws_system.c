/*
 * ws_system.c
 *
 *  Created on: 27.12.2019
 *      Author: Damian Plonek
 */


#include "hwal/ws_system.h"
#include "hwal/ws_digital_input.h"
#include "hwal/ws_button.h"
#include "hwal/ws_timer.h"
#include "ws_configuration_write.h"
#include "winsens_config.h"
#define WS_LOG_MODULE_NAME " SYS"
#include "ws_log.h"

#include "fstorage.h"
#include "softdevice_handler.h"
#include "ble_advertising.h"


#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_XTAL,            \
                                 .rc_ctiv       = 0,                                \
                                 .rc_temp_ctiv  = 0,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}
#define CENTRAL_LINK_COUNT              0                                           /**< Number of central links used by the application. When changing this number remember to adjust the RAM settings*/
#define PERIPHERAL_LINK_COUNT           1                                           /**< Number of peripheral links used by the application. When changing this number remember to adjust the RAM settings*/


#define WS_SYSTEM_BUTTON_HOLD_DURATION          20 // it is 2000ms in 100ms unit


static void ws_sys_evt_dispatch(
    uint32_t sys_evt);
static void WS_DigitalInputCallback(
    WS_DigitalInputPins_e pin,
    WS_ButtonPushType_e pushType);
static void WS_TimerCallback(
    WS_TimerId_t timerId);

static WS_TimerId_t ws_systemTimer = 0;
static uint32_t ws_timeCounter = 0;

WINSENS_Status_e WS_SystemInit(void)
{
    uint32_t err_code;
    WINSENS_Status_e status = WINSENS_ERROR;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
                                                    PERIPHERAL_LINK_COUNT,
                                                    &ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    err_code = softdevice_sys_evt_handler_set(ws_sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    status = WS_TimerInit();
    WS_LOG_ERROR_CHECK(status);
    status = WS_DigitalInputInit();
    WS_LOG_ERROR_CHECK(status);
    status = WS_ButtonInit();
    WS_LOG_ERROR_CHECK(status);

    status = WS_TimerSetTimer(1, WS_TimerCallback, &ws_systemTimer);
    WS_LOG_ERROR_CHECK(status);

    status = WS_ButtonRegisterCallback(WS_DIGITAL_INPUT_PAIR_BTN, WS_DigitalInputCallback);
    WS_LOG_ERROR_CHECK(status);

    return (NRF_SUCCESS == err_code) ? WINSENS_OK : WINSENS_ERROR;
}

void WS_SystemDeinit(void)
{
    WS_DigitalInputUnregisterCallback(WS_DIGITAL_INPUT_PAIR_BTN);
    WS_TimerCancel(ws_systemTimer);

    WS_DigitalInputDeinit();
    WS_TimerDeinit();
}

uint32_t WS_SystemGetTime(void)
{
    return ws_timeCounter;
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
    WS_DigitalInputPins_e pin,
    WS_ButtonPushType_e pushType)
{
    if (WS_DIGITAL_INPUT_PAIR_BTN == pin &&
        WS_BUTTON_PUSH_LONG == pushType)
    {
        const WS_Configuration_t *config = WS_ConfigurationGet();
        if (config->bonded)
        {
            WS_Configuration_t newConfig = *config;
            newConfig.bonded = false;
            memset(&newConfig.address, 0xFF, WS_CONFIGURATION_ADDR_LEN);
            WS_ConfigurationSet(&newConfig);
        }
    }
}

static void WS_TimerCallback(
    WS_TimerId_t timerId)
{
    ++ws_timeCounter;
}

