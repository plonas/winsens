/*
 * spi.c
 *
 *  Created on: 14.04.2021
 *      Author: Damian Plonek
 */


#include "spi.h"
#include "spi_cfg.h"
#include "task_queue.h"
#define ILOG_MODULE_NAME SPI
#include "log.h"
#include "log_internal_nrf52.h"
#include "nrfx_spi.h"


void event_handler(nrfx_spi_evt_t const *p_event, void *p_context);
void task_function(void *p_data, uint16_t data_size);


static bool                     g_initialized                           = false;
static nrfx_spi_t               g_spi                                   = NRFX_SPI_INSTANCE(0);
static winsens_event_handler_t  g_subscribers[SPI_CFG_MAX_SUBSCRIBERS]  = {NULL};


LOG_REGISTER();


winsens_status_t spi_init(void)
{
    if (!g_initialized)
    {
        g_initialized = true;

        nrfx_spi_config_t spi_config    = NRFX_SPI_DEFAULT_CONFIG;
        spi_config.sck_pin              = SPI_CFG_SCK_PIN;
        spi_config.miso_pin             = SPI_CFG_MISO_PIN;
        spi_config.mosi_pin             = SPI_CFG_MOSI_PIN;
        spi_config.ss_pin               = SPI_CFG_SS_PIN;
        spi_config.frequency            = SPI_CFG_FREQUENCY;

        task_queue_init();
        nrfx_spi_init(&g_spi, &spi_config, event_handler, NULL);
    }

    return WINSENS_OK;
}

winsens_status_t spi_subscribe(winsens_event_handler_t event_handler)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    for (int i = 0; i < SPI_CFG_MAX_SUBSCRIBERS; ++i)
    {
        if (NULL == g_subscribers[i])
        {
            g_subscribers[i] = event_handler;
            return WINSENS_OK;
        }
    }

    return WINSENS_NO_RESOURCES;
}

winsens_status_t spi_transfer(uint8_t *tx, uint16_t tx_len, uint8_t *rx, uint16_t rx_len)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    nrfx_spi_xfer_desc_t desc = { .p_rx_buffer = rx, .rx_length = rx_len, .p_tx_buffer = tx, .tx_length = tx_len };
    nrfx_spi_xfer(&g_spi, &desc, 0);

    return WINSENS_OK;
}

void event_handler(nrfx_spi_evt_t const *p_event, void *p_context)
{
    if (NRFX_SPI_EVENT_DONE == p_event->type)
    {
        task_queue_add(NULL, 0, task_function);
    }
}

void task_function(void *p_data, uint16_t data_size)
{
    for (int i = 0; i < SPI_CFG_MAX_SUBSCRIBERS; ++i)
    {
        if (g_subscribers[i])
        {
            winsens_event_t e = { .id = SPI_EVT_TRANSFER_DONE, .data = 0 };
            g_subscribers[i](e);
        }
    }
}
