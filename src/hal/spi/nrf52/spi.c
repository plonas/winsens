/*
 * spi.c
 *
 *  Created on: 14.04.2021
 *      Author: Damian Plonek
 */


#include "spi.h"
#include "spi_cfg.h"
#include "task_queue.h"
#define ILOG_MODULE_NAME WSPI
#include "log.h"
#include "log_internal_nrf52.h"
#include "nrfx_spi.h"


#define SPI_CFG_SIZE        (sizeof(g_spi_cfg)/sizeof(g_spi_cfg[0]))
#define SPI_CFG_NRF_SIZE    (sizeof(g_spi_nrf_cfg)/sizeof(g_spi_nrf_cfg[0]))


void event_handler(nrfx_spi_evt_t const *p_event, void *p_context);


static bool                     g_initialized       = false;
static nrfx_spi_t               g_spi_nrf_cfg[]     = SPI_CFG_NRF_INIT;
static spi_cfg_t                g_spi_cfg[]         = SPI_CFG_INIT;

LOG_REGISTER();


winsens_status_t spi_init(void)
{
    if (!g_initialized)
    {
        g_initialized = true;

        LOG_ERROR_BOOL_RETURN(SPI_CFG_NRF_SIZE == SPI_CFG_SIZE, WINSENS_INVALID_PARAMS);

        for (int i = 0; i < SPI_CFG_SIZE; ++i)
        {
            nrfx_spi_config_t spi_config    = NRFX_SPI_DEFAULT_CONFIG;
            spi_config.sck_pin              = g_spi_cfg[i].sck_pin;
            spi_config.miso_pin             = g_spi_cfg[i].miso_pin;
            spi_config.mosi_pin             = g_spi_cfg[i].mosi_pin;
            spi_config.ss_pin               = g_spi_cfg[i].ss_pin;
            spi_config.frequency            = g_spi_cfg[i].freq;
            spi_config.orc                  = 0x00;

            nrfx_err_t err = nrfx_spi_init(&g_spi_nrf_cfg[i], &spi_config, event_handler, (void*)i);
            LOG_NRF_DEBUG_CHECK(err);
            LOG_FLUSH();
        }

        task_queue_init();
    }

    return WINSENS_OK;
}

winsens_status_t spi_subscribe(spi_t spi, winsens_event_handler_t evt_handler)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_ERROR_BOOL_RETURN(SPI_CFG_SIZE > spi, WINSENS_INVALID_PARAMS);

    g_spi_cfg[spi].evt_handler = evt_handler;

    return WINSENS_OK;
}

winsens_status_t spi_transfer(spi_t spi, uint8_t *tx, uint16_t tx_len, uint8_t *rx, uint16_t rx_len)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    nrfx_spi_xfer_desc_t desc = { .p_rx_buffer = rx, .rx_length = rx_len, .p_tx_buffer = tx, .tx_length = tx_len };
    nrfx_err_t err = nrfx_spi_xfer(&g_spi_nrf_cfg[spi], &desc, 0);
    LOG_NRF_DEBUG_CHECK(err);
    LOG_FLUSH();

    return WINSENS_OK;
}

void event_handler(nrfx_spi_evt_t const *p_event, void *p_context)
{
    if (NRFX_SPI_EVENT_DONE == p_event->type)
    {
        const spi_t spi = (spi_t)(uint32_t)p_context;
        if (g_spi_cfg[spi].evt_handler)
        {
            winsens_event_t e = { .id = SPI_EVT_TRANSFER_DONE, .data = 0 };
            g_spi_cfg[spi].evt_handler(e);
        }
    }
}
