/*
 * ConfigStorage.c
 *
 *  Created on: 13.05.2019
 *      Author: Damian Plonek
 */


#include "config.h"
#include "storage.h"
#define ILOG_MODULE_NAME CFG
#include "log.h"


#define CONFIGURATION_STORAGE_ID                0x000B

#define CONFIGURATION_THRESHOLD_DEFAULT         400

static config_t ws_configuration = {
    { true, false },
    { CONFIGURATION_THRESHOLD_DEFAULT, CONFIGURATION_THRESHOLD_DEFAULT },
    false,
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
};


winsens_status_t config_init(void)
{
    winsens_status_t status = storage_init();
    if (WINSENS_OK != status)
    {
        return status;
    }

    status = storage_read(CONFIGURATION_STORAGE_ID, sizeof(config_t), (uint8_t *) &ws_configuration);
    LOG_INFO("IConfig_Init status %u", status);
    if (WINSENS_NOT_FOUND == status)
    {
        // keep the default configuration and store it
        status = storage_write(CONFIGURATION_STORAGE_ID, sizeof(config_t), (uint8_t *) &ws_configuration);
    }

    LOG_FLUSH();
    return status;
}

const config_t * config_get(void)
{
    winsens_status_t status = storage_read(CONFIGURATION_STORAGE_ID, sizeof(config_t), (uint8_t *) &ws_configuration);
    LOG_WARNING_CHECK(status);
    LOG_FLUSH();
    return &ws_configuration;
}

winsens_status_t config_set(
    const config_t *configuration)
{
    winsens_status_t status = storage_write(CONFIGURATION_STORAGE_ID, sizeof(config_t), (uint8_t *) configuration);
    LOG_WARNING_CHECK(status);
    if (WINSENS_OK == status)
    {
        ws_configuration = *configuration;
    }

    LOG_FLUSH();
    return status;
}

