/*
 * ConfigStorage.c
 *
 *  Created on: 13.05.2019
 *      Author: Damian Plonek
 */


#include "IConfig.h"
#include "ConfigStorage.h"
#include "ws_storage.h"
#define WS_LOG_MODULE_NAME CFG
#include "ws_log.h"


#ifdef WINSENS_IF_CONFIG_STORAGE
#define ConfigStorage_Init          IConfig_Init
#define ConfigStorage_Get           IConfig_Get
#define ConfigStorage_Set           IConfig_Set
#endif


#define WS_CONFIGURATION_STORAGE_ID             0x000B

#define WS_CONFIGURATION_THRESHOLD_DEFAULT      400

static WS_Configuration_t ws_configuration = {
    { true, false },
    { WS_CONFIGURATION_THRESHOLD_DEFAULT, WS_CONFIGURATION_THRESHOLD_DEFAULT },
    false,
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
};


WINSENS_Status_e ConfigStorage_Init(void)
{
    WINSENS_Status_e status = WS_StorageInit();
    if (WINSENS_OK != status)
    {
        return status;
    }

    status = WS_StorageRead(WS_CONFIGURATION_STORAGE_ID, sizeof(WS_Configuration_t), (uint8_t *) &ws_configuration);
    WS_LOG_INFO("ConfigStorage_Init status %u", status);
    if (WINSENS_NOT_FOUND == status)
    {
        // keep the default configuration and store it
        status = WS_StorageWrite(WS_CONFIGURATION_STORAGE_ID, sizeof(WS_Configuration_t), (uint8_t *) &ws_configuration);
    }

    WS_LOG_FLUSH();
    return status;
}

const WS_Configuration_t * ConfigStorage_Get(void)
{
    WINSENS_Status_e status = WS_StorageRead(WS_CONFIGURATION_STORAGE_ID, sizeof(WS_Configuration_t), (uint8_t *) &ws_configuration);
    WS_LOG_WARNING_CHECK(status);
    WS_LOG_FLUSH();
    return &ws_configuration;
}

WINSENS_Status_e ConfigStorage_Set(
    const WS_Configuration_t *configuration)
{
    WINSENS_Status_e status = WS_StorageWrite(WS_CONFIGURATION_STORAGE_ID, sizeof(WS_Configuration_t), (uint8_t *) configuration);
    WS_LOG_WARNING_CHECK(status);
    if (WINSENS_OK == status)
    {
        ws_configuration = *configuration;
    }

    WS_LOG_FLUSH();
    return status;
}

