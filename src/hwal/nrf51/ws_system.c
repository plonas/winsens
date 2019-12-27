/*
 * ws_system.c
 *
 *  Created on: 27.12.2019
 *      Author: Damian Plonek
 */


#include "hwal/ws_system.h"
#include "softdevice_handler.h"

#include "fstorage.h"
#include "ble_advertising.h"


static void ws_sys_evt_dispatch(uint32_t sys_evt);

WINSENS_Status_e WS_SystemInit(void)
{
    uint32_t err_code = softdevice_sys_evt_handler_set(ws_sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    return (NRF_SUCCESS == err_code) ? WINSENS_OK : WINSENS_ERROR;
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
