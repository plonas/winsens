/*
 * ConfigStorage.c
 *
 *  Created on: 13.05.2019
 *      Author: Damian Plonek
 */


#include "config.h"
#include "config_cfg.h"
#include "storage.h"
#include "storage_cfg.h"
#define ILOG_MODULE_NAME CFG
#include "log.h"


static bool g_initialized = false;


LOG_REGISTER();


winsens_status_t config_init(void)
{
    winsens_status_t status = WINSENS_OK;

    if (false == g_initialized)
    {
        g_initialized = true;

        status = storage_init();
        LOG_WARNING_CHECK(status);
    }

    return status;
}

void config_get(config_id_t id, void *config, uint16_t config_size, const void *default_config)
{
    if (!g_initialized)
    {
        return;
    }

    winsens_status_t status = storage_read(STORAGE_FILE_ID_STORAGE, id, config_size, config);

    if (WINSENS_OK != status)
    {
        memcpy(config, default_config, config_size);
        LOG_WARNING("Using default config for %u (error: %d)", id, status);
    }
}

winsens_status_t config_set(config_id_t id, const void *config, uint16_t config_size)
{
    if (!g_initialized)
    {
        return WINSENS_NOT_INITIALIZED;
    }

    winsens_status_t status = storage_write(STORAGE_FILE_ID_STORAGE, id, config_size, (uint8_t *) config);
    LOG_WARNING_CHECK(status);

    return status;
}

