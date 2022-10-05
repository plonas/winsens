/*
 * i2c.c
 *
 *  Created on: 02.07.2022
 *      Author: Damian Plonek
 */


#include "i2c.h"
#include "i2c_cfg.h"
#include "task_queue.h"
#include "pwr_mgr.h"
#define ILOG_MODULE_NAME WI2C
#include "log.h"
#include "log_internal_nrf52.h"
#include "nrfx_twi.h"


#define I2C_CFG_SIZE        (sizeof(g_i2c_cfg)/sizeof(g_i2c_cfg[0]))
#define I2C_CFG_NRF_SIZE    (sizeof(g_twi_nrf_cfg)/sizeof(g_twi_nrf_cfg[0]))


static void event_handler(nrfx_twi_evt_t const *p_event, void *p_context);
static void pwr_mgr_evt_handler(winsens_event_t event);


static bool                     g_initialized       = false;
static nrfx_twi_t               g_twi_nrf_cfg[]     = I2C_CFG_NRF_INIT;
static i2c_cfg_t                g_i2c_cfg[]         = I2C_CFG_INIT;

LOG_REGISTER();


winsens_status_t i2c_init(void)
{
    if (!g_initialized)
    {
        g_initialized = true;

        LOG_ERROR_BOOL_RETURN(I2C_CFG_NRF_SIZE == I2C_CFG_SIZE, WINSENS_INVALID_PARAMS);

        for (int i = 0; i < I2C_CFG_SIZE; ++i)
        {
            nrfx_twi_config_t twi_config    = NRFX_TWI_DEFAULT_CONFIG;
            twi_config.hold_bus_uninit      = false;
            twi_config.frequency            = g_i2c_cfg[i].freq;
            twi_config.scl                  = g_i2c_cfg[i].scl_pin;
            twi_config.sda                  = g_i2c_cfg[i].sda_pin;

            nrfx_err_t err = nrfx_twi_init(&g_twi_nrf_cfg[i], &twi_config, event_handler, (void*)i);
            LOG_NRF_DEBUG_CHECK(err);
            nrfx_twi_enable(&g_twi_nrf_cfg[i]);
        }

        task_queue_init();
        pwr_mgr_init();
        pwr_mgr_subscribe(pwr_mgr_evt_handler);
    }

    return WINSENS_OK;
}

winsens_status_t i2c_subscribe(i2c_t i2c, winsens_event_handler_t evt_handler)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_ERROR_BOOL_RETURN(I2C_CFG_SIZE > i2c, WINSENS_INVALID_PARAMS);

    g_i2c_cfg[i2c].evt_handler = evt_handler;

    return WINSENS_OK;
}

#include "nrf_log.h"
winsens_status_t i2c_tx(i2c_t i2c, uint8_t *tx, uint16_t len)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    nrfx_twi_xfer_desc_t desc = NRFX_TWI_XFER_DESC_TX(g_i2c_cfg[i2c].addr, tx, len);

    nrfx_err_t err = nrfx_twi_xfer(&g_twi_nrf_cfg[i2c], &desc, 0);
    LOG_NRF_DEBUG_CHECK(err);
    // LOG_FLUSH();
    // LOG_DEBUG("tx");
    // NRF_LOG_HEXDUMP_DEBUG(tx, len);

    return WINSENS_OK;
}

winsens_status_t i2c_rx(i2c_t i2c, uint8_t *rx, uint16_t len)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    nrfx_twi_xfer_desc_t desc = NRFX_TWI_XFER_DESC_RX(g_i2c_cfg[i2c].addr, rx, len);
    
    nrfx_err_t err = nrfx_twi_xfer(&g_twi_nrf_cfg[i2c], &desc, 0);
    LOG_NRF_DEBUG_CHECK(err);
    // LOG_FLUSH();
    // LOG_DEBUG("rx");

    return WINSENS_OK;
}

winsens_status_t i2c_txtx(i2c_t i2c, uint8_t *tx1, uint16_t len1, uint8_t *tx2, uint16_t len2)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    nrfx_twi_xfer_desc_t desc = NRFX_TWI_XFER_DESC_TXTX(g_i2c_cfg[i2c].addr, tx1, len1, tx2, len2);

    nrfx_err_t err = nrfx_twi_xfer(&g_twi_nrf_cfg[i2c], &desc, 0);
    LOG_NRF_DEBUG_CHECK(err);
    // LOG_FLUSH();
    // LOG_DEBUG("txtx");
    // NRF_LOG_HEXDUMP_DEBUG(tx1, len1);
    // NRF_LOG_HEXDUMP_DEBUG(tx2, len2);

    return WINSENS_OK;
}

winsens_status_t i2c_txrx(i2c_t i2c, uint8_t *tx, uint16_t len_tx, uint8_t *rx, uint16_t len_rx)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    nrfx_twi_xfer_desc_t desc = NRFX_TWI_XFER_DESC_TXRX(g_i2c_cfg[i2c].addr, tx, len_tx, rx, len_rx);

    nrfx_err_t err = nrfx_twi_xfer(&g_twi_nrf_cfg[i2c], &desc, 0);
    LOG_NRF_DEBUG_CHECK(err);
    // LOG_FLUSH();
    // LOG_DEBUG("txrx");
    // NRF_LOG_HEXDUMP_DEBUG(tx, len_tx);

    return WINSENS_OK;
}

static void event_handler(nrfx_twi_evt_t const *p_event, void *p_context)
{
    // LOG_DEBUG("TWI evt: %u", p_event->type);
    if (NRFX_TWI_EVT_DONE == p_event->type)
    {
        if (p_event->xfer_desc.primary_length)
        {
            // NRF_LOG_HEXDUMP_DEBUG(p_event->xfer_desc.p_primary_buf, p_event->xfer_desc.primary_length);
        }
        if (p_event->xfer_desc.secondary_length)
        {
            // NRF_LOG_HEXDUMP_DEBUG(p_event->xfer_desc.p_secondary_buf, p_event->xfer_desc.secondary_length);
        }

        const i2c_t i2c = (i2c_t)(uint32_t)p_context;
        if (g_i2c_cfg[i2c].evt_handler)
        {
            winsens_event_t e = { .id = I2C_EVT_TRANSFER_DONE, .data = 0 };
            g_i2c_cfg[i2c].evt_handler(e);
        }
    }
}

static void pwr_mgr_evt_handler(winsens_event_t event)
{
    if (PWR_MGR_EVT_PREPARE_TO_SLEEP == event.id ||
        PWR_MGR_EVT_PREPARE_TO_SHUTDOWN == event.id)
    {
        for (int i = 0; i < I2C_CFG_SIZE; ++i)
        {
            nrfx_twi_uninit(&g_twi_nrf_cfg[i]);
        }
    }
}