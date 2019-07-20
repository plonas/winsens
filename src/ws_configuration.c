/*
 * configuration.c
 *
 *  Created on: 13.05.2019
 *      Author: Damian Plonek
 */


#include "ws_configuration.h"

#include "hwal/ws_storage.h"

#define WS_CONFIGURATION_STORAGE_ID             0xA100

#define WS_CONFIGURATION_THRESHOLD_DEFAULT      400

static WS_Configuration_t ws_configuration = {
    { true, false },
    { WS_CONFIGURATION_THRESHOLD_DEFAULT, WS_CONFIGURATION_THRESHOLD_DEFAULT }
};


WINSENS_Status_e WS_ConfigurationInit(void)
{
    WINSENS_Status_e status = WS_StorageRead(WS_CONFIGURATION_STORAGE_ID, sizeof(WS_Configuration_t), (uint8_t *) &ws_configuration);
    if (WINSENS_NOT_FOUND == status)
    {
        // keep the default configuration and store it
        status = WS_StorageWrite(WS_CONFIGURATION_STORAGE_ID, sizeof(WS_Configuration_t), (uint8_t *) &ws_configuration);
    }

    return status;
}

const WS_Configuration_t * WS_ConfigurationGet(void)
{
    return &ws_configuration;
}

WINSENS_Status_e WS_ConfigurationSet(
    const WS_Configuration_t *configuration)
{
    WINSENS_Status_e status = WS_StorageWrite(WS_CONFIGURATION_STORAGE_ID, sizeof(WS_Configuration_t), (uint8_t *) configuration);
    if (WINSENS_OK == status)
    {
        ws_configuration = *configuration;
    }

    return status;
}

