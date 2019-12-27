/*
 * configuration.c
 *
 *  Created on: 13.05.2019
 *      Author: Damian Plonek
 */


#include "ws_configuration.h"

#include "hwal/ws_storage.h"

#define NRF_LOG_MODULE_NAME "CFG"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define WS_CONFIGURATION_STORAGE_ID             0x000B

#define WS_CONFIGURATION_THRESHOLD_DEFAULT      400

static WS_Configuration_t ws_configuration = {
    { true, false },
    { WS_CONFIGURATION_THRESHOLD_DEFAULT, WS_CONFIGURATION_THRESHOLD_DEFAULT }
};


WINSENS_Status_e WS_ConfigurationInit(void)
{
    WINSENS_Status_e status = WS_StorageInit();
    if (WINSENS_OK != status)
    {
        return status;
    }

    status = WS_StorageRead(WS_CONFIGURATION_STORAGE_ID, sizeof(WS_Configuration_t), (uint8_t *) &ws_configuration);
    NRF_LOG_INFO("WS_ConfigurationInit status %u\n", status);
    NRF_LOG_FLUSH();
    if (WINSENS_NOT_FOUND == status)
    {
        // keep the default configuration and store it
        status = WS_StorageWrite(WS_CONFIGURATION_STORAGE_ID, sizeof(WS_Configuration_t), (uint8_t *) &ws_configuration);
    }

    return status;
}

const WS_Configuration_t * WS_ConfigurationGet(void)
{
    WINSENS_Status_e status = WS_StorageRead(WS_CONFIGURATION_STORAGE_ID, sizeof(WS_Configuration_t), (uint8_t *) &ws_configuration);
    NRF_LOG_INFO("WS_ConfigurationGet status %u\n", status);
    NRF_LOG_FLUSH();
    return &ws_configuration;
}

WINSENS_Status_e WS_ConfigurationSet(
    const WS_Configuration_t *configuration)
{
    WINSENS_Status_e status = WS_StorageWrite(WS_CONFIGURATION_STORAGE_ID, sizeof(WS_Configuration_t), (uint8_t *) configuration);
    NRF_LOG_INFO("WS_ConfigurationSet status %u\n", status);
    NRF_LOG_FLUSH();
    if (WINSENS_OK == status)
    {
        ws_configuration = *configuration;
    }

    return status;
}

