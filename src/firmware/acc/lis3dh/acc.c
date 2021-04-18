/*
 * acc.c
 *
 *  Created on: 16.04.2021
 *      Author: Damian Plonek
 */


#include "acc.h"
#include "acc_cfg.h"
#include "spi.h"
#include "digital_io.h"
#include "digital_io_cfg.h"
#define ILOG_MODULE_NAME ACC
#include "log.h"


//LIS3DH Registers
#define LIS3DH_STATUS_REG_AUX           0x07
#define LIS3DH_OUT_ADC1_L               0x08
#define LIS3DH_OUT_ADC1_H               0x09
#define LIS3DH_OUT_ADC2_L               0x0A
#define LIS3DH_OUT_ADC2_H               0x0B
#define LIS3DH_OUT_ADC3_L               0x0C
#define LIS3DH_OUT_ADC3_H               0x0D
#define LIS3DH_INT_COUNTER_REG          0x0E
#define LIS3DH_WHO_AM_I                 0x0F

#define LIS3DH_TEMP_CFG_REG             0x1F
#define LIS3DH_CTRL_REG1                0x20
#define LIS3DH_CTRL_REG2                0x21
#define LIS3DH_CTRL_REG3                0x22
#define LIS3DH_CTRL_REG4                0x23
#define LIS3DH_CTRL_REG5                0x24
#define LIS3DH_CTRL_REG6                0x25
#define LIS3DH_REFERENCE                0x26
#define LIS3DH_STATUS_REG2              0x27
#define LIS3DH_OUT_X_L                  0x28
#define LIS3DH_OUT_X_H                  0x29
#define LIS3DH_OUT_Y_L                  0x2A
#define LIS3DH_OUT_Y_H                  0x2B
#define LIS3DH_OUT_Z_L                  0x2C
#define LIS3DH_OUT_Z_H                  0x2D
#define LIS3DH_FIFO_CTRL_REG            0x2E
#define LIS3DH_FIFO_SRC_REG             0x2F
#define LIS3DH_INT1_CFG                 0x30
#define LIS3DH_INT1_SRC                 0x31
#define LIS3DH_INT1_THS                 0x32
#define LIS3DH_INT1_DURATION            0x33

#define LIS3DH_CLICK_CFG                0x38
#define LIS3DH_CLICK_SRC                0x39
#define LIS3DH_CLICK_THS                0x3A
#define LIS3DH_TIME_LIMIT               0x3B
#define LIS3DH_TIME_LATENCY             0x3C
#define LIS3DH_TIME_WINDOW              0x3D

//#define LIS3DH_TEMP_CFG_REG_VAL         0b00000000
#define LIS3DH_CTRL_REG1_VAL            0b00101111
//#define LIS3DH_CTRL_REG2_VAL            0b00101111
#define LIS3DH_CTRL_REG3_VAL            0b00000100
//#define LIS3DH_CTRL_REG4_VAL            0b00010000
#define LIS3DH_CTRL_REG5_VAL            0b01000000
//#define LIS3DH_CTRL_REG6_VAL            0b00000000
#define LIS3DH_FIFO_CTRL_REG_VAL        0b10001010

#define LIS3DH_REG_BITMASK              0b00111111
#define LIS3DH_FSS_BITMASK              0b00011111

#define LIS3DH_READ_REQ_BIT             0x80
#define LIS3DH_AUTO_INCREMENT_BIT       0x40

#define LIS3DH_IS_READ_BIT_SET(x)       ((x) & LIS3DH_READ_REQ_BIT)
#define LIS3DH_GET_REG(x)               ((x) & LIS3DH_REG_BITMASK)

#define ACC_RX_BUFFER_SIZE              (196)
#define ACC_TX_BUFFER_SIZE              (8)
#define ACC_DATA_BUFFER_LEN             (12)


typedef enum
{
    ACC_STATE_IDLE,
    ACC_STATE_READ_FIFO,
} acc_state_t;


static void event_handler(winsens_event_t event);
static void dio_callback(digital_io_input_pin_t pin, bool on);
static void lis3dh_init(void);
static void write_reg(uint8_t reg, uint8_t value);
static void read_reg(uint8_t reg);
static void read_multiple_bytes(uint8_t reg, uint16_t len);
static void read_fifo(void);
static void store_acc_data(uint8_t* data, uint16_t len);
static void update_subscribers(void);


static bool                     g_initialized                           = false;
static acc_state_t              g_current_state                         = ACC_STATE_IDLE;
static uint8_t                  g_fss                                   = 0; // fifo stored samples

static winsens_event_handler_t  g_subscribers[ACC_CFG_MAX_SUBSCRIBERS]  = {NULL};
static uint8_t                  g_rx_buffer[ACC_RX_BUFFER_SIZE];
static uint8_t                  g_tx_buffer[ACC_TX_BUFFER_SIZE];
static acc_data_t               g_acc_data_buf[ACC_DATA_BUFFER_LEN];
static uint16_t                 g_acc_data_len                          = 0;


winsens_status_t acc_init(void)
{
    if (!g_initialized)
    {
        g_initialized = true;

        spi_init();
        spi_subscribe(event_handler);

        digital_io_register_callback(DIGITAL_IO_INPUT_ACC_FIFO_INT, dio_callback);

        lis3dh_init();
    }

    return WINSENS_OK;
}

winsens_status_t acc_subscribe(winsens_event_handler_t event_handler)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    for (int i = 0; i < ACC_CFG_MAX_SUBSCRIBERS; ++i)
    {
        if (NULL == g_subscribers[i])
        {
            g_subscribers[i] = event_handler;
            return WINSENS_OK;
        }
    }

    return WINSENS_NO_RESOURCES;
}

const acc_data_t* acc_get_data(uint16_t* len)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, NULL);

    *len = g_acc_data_len;
    return g_acc_data_buf;
}

static void event_handler(winsens_event_t event)
{
    if (SPI_EVT_TRANSFER_DONE == event.id)
    {
        switch (g_current_state)
        {
            case ACC_STATE_READ_FIFO:
                switch (LIS3DH_GET_REG(g_tx_buffer[0]))
                {
                case LIS3DH_FIFO_SRC_REG:
                    g_fss = g_rx_buffer[0] & LIS3DH_FSS_BITMASK;
                    read_multiple_bytes(LIS3DH_OUT_X_L, g_fss * 6);
                    break;

                case LIS3DH_OUT_X_L:
                    store_acc_data(g_rx_buffer, g_fss);
                    g_current_state = ACC_STATE_IDLE;
                    update_subscribers();
                    break;

                default:
                    break;
                }
                break;

            default:
                break;
        }
    }
}

static void dio_callback(digital_io_input_pin_t pin, bool on)
{
    if (DIGITAL_IO_INPUT_ACC_FIFO_INT == pin && on)
    {
        read_fifo();
    }
}

static void lis3dh_init(void)
{
    write_reg(LIS3DH_CTRL_REG1, LIS3DH_CTRL_REG1_VAL);
    write_reg(LIS3DH_CTRL_REG3, LIS3DH_CTRL_REG3_VAL);
    write_reg(LIS3DH_CTRL_REG5, LIS3DH_CTRL_REG5_VAL);
    write_reg(LIS3DH_FIFO_CTRL_REG, LIS3DH_FIFO_CTRL_REG_VAL);
}

static void write_reg(uint8_t reg, uint8_t value)
{
    g_tx_buffer[0] = reg;
    g_tx_buffer[1] = value;
    spi_transfer(g_tx_buffer, 2, g_rx_buffer, 0);
}

static void read_reg(uint8_t reg)
{
    g_tx_buffer[0] = reg | LIS3DH_READ_REQ_BIT;
    spi_transfer(g_tx_buffer, 1, g_rx_buffer, 1);
}

static void read_multiple_bytes(uint8_t reg, uint16_t len)
{
    g_tx_buffer[0] = reg | LIS3DH_READ_REQ_BIT | LIS3DH_AUTO_INCREMENT_BIT;
    spi_transfer(g_tx_buffer, 1, g_rx_buffer, len);
}

static void read_fifo(void)
{
    g_current_state = ACC_STATE_READ_FIFO;
    read_reg(LIS3DH_FIFO_SRC_REG);
}

static void store_acc_data(uint8_t* data, uint16_t len)
{
    uint16_t i = 0;
    uint16_t n = 0;

    while (i < ACC_DATA_BUFFER_LEN && n < len)
    {
        g_acc_data_buf[i].x = *((uint16_t*)&data[n]);
        n += 2;
        g_acc_data_buf[i].y = *((uint16_t*)&data[n]);
        n += 2;
        g_acc_data_buf[i].z = *((uint16_t*)&data[n]);
        n += 2;
        i++;
    }

    g_acc_data_len = i;
}

static void update_subscribers(void)
{
    winsens_event_t e = { .id = ACC_EVT_NEW_DATA, .data = 0 };

    for (int i = 0; i < ACC_CFG_MAX_SUBSCRIBERS; ++i)
    {
        if (g_subscribers[i])
        {
            g_subscribers[i](e);
        }
    }
}
