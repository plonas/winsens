/*
 * spi.c
 *
 *  Created on: 14.04.2021
 *      Author: Damian Plonek
 */


#include "spi.h"
#include "spi_cfg.h"
#include "circular_buf.h"
#include "task_queue.h"
#define ILOG_MODULE_NAME SPI
#include "log.h"
#include "log_internal_nrf52.h"
#include "nrfx_spi.h"


#define SPI_RX_CHUNK_SIZE       (16)
#define SPI_TX_CHUNK_SIZE       (16)


void event_handler(nrfx_spi_evt_t const *p_event, void *p_context);
void task_function(void *p_data, uint16_t data_size);


static bool                     g_initialized                           = false;
static nrfx_spi_t               g_spi                                   = NRFX_SPI_INSTANCE(0);
static winsens_event_handler_t  g_subscribers[SPI_CFG_MAX_SUBSCRIBERS]  = {NULL};
static circular_buf_t           g_rx_buffer                             = {0};
static circular_buf_t           g_tx_buffer                             = {0};
static uint8_t                  g_rx_data_buffer[SPI_CFG_RX_BUFFER_SIZE];
static uint8_t                  g_tx_data_buffer[SPI_CFG_TX_BUFFER_SIZE];
static uint8_t                  g_rx_chunk[SPI_RX_CHUNK_SIZE];
static uint8_t                  g_tx_chunk[SPI_TX_CHUNK_SIZE];


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

        circular_buf_init(&g_rx_buffer, g_rx_data_buffer, SPI_CFG_RX_BUFFER_SIZE);
        circular_buf_init(&g_tx_buffer, g_tx_data_buffer, SPI_CFG_TX_BUFFER_SIZE);

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

winsens_status_t spi_write(uint8_t *data, uint16_t len)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    uint32_t copied = circular_buf_push(&g_tx_buffer, data, len);
    LOG_ERROR_BOOL_RETURN(copied == len, WINSENS_NO_RESOURCES);

    nrfx_spi_xfer_desc_t desc = { .p_rx_buffer = g_rx_chunk, .rx_length = SPI_RX_CHUNK_SIZE, .p_tx_buffer = g_tx_chunk, .tx_length = SPI_TX_CHUNK_SIZE };
    desc.tx_length = circular_buf_pop(&g_tx_buffer, g_tx_chunk, SPI_TX_CHUNK_SIZE);
    nrfx_spi_xfer(&g_spi, &desc, 0);

    return WINSENS_OK;
}

uint32_t spi_read(uint8_t *buffer, uint16_t len)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, 0);

    return circular_buf_pop(&g_rx_buffer, buffer, len);
}

void event_handler(nrfx_spi_evt_t const *p_event, void *p_context)
{
    if (NRFX_SPI_EVENT_DONE == p_event->type)
    {
        uint32_t copied = circular_buf_push(&g_rx_buffer, p_event->xfer_desc.p_rx_buffer, p_event->xfer_desc.rx_length);
        LOG_IF_ERROR(copied != p_event->xfer_desc.rx_length, "RX circular buffer full");

        nrfx_spi_xfer_desc_t desc = { .p_rx_buffer = g_rx_chunk, .rx_length = SPI_RX_CHUNK_SIZE, .p_tx_buffer = NULL, .tx_length = 0 };
        if (0 != circular_buf_size(&g_tx_buffer))
        {
            desc.p_tx_buffer = g_tx_chunk;
            desc.tx_length = circular_buf_pop(&g_tx_buffer, g_tx_chunk, SPI_TX_CHUNK_SIZE);
        }

        task_queue_add(NULL, 0, task_function);

        nrfx_spi_xfer(&g_spi, &desc, 0);
    }
}

void task_function(void *p_data, uint16_t data_size)
{
    for (int i = 0; i < SPI_CFG_MAX_SUBSCRIBERS; ++i)
    {
        if (g_subscribers[i])
        {
            winsens_event_t e = { .id = SPI_EVT_READ_DATA, .data = 0 };
            g_subscribers[i](e);
        }
    }
}
