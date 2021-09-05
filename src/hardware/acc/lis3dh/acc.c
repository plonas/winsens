/*
 * acc.c
 *
 *  Created on: 16.04.2021
 *      Author: Damian Plonek
 */


#include "acc.h"
#include "acc_cfg.h"
#include "command.h"
#include "circular_buf_safe.h"
#include "critical_region.h"
#include "digital_io.h"
#include "digital_io_cfg.h"
#include "spi.h"
#include "spi_cfg.h"
#include "subscribers.h"
#include "task_queue.h"
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

#define LIS3DH_CTRL_REG0                0x1E
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
#define LIS3DH_INT2_CFG                 0x34
#define LIS3DH_INT2_SRC                 0x35
#define LIS3DH_INT2_THS                 0x36
#define LIS3DH_INT2_DURATION            0x37

#define LIS3DH_CLICK_CFG                0x38
#define LIS3DH_CLICK_SRC                0x39
#define LIS3DH_CLICK_THS                0x3A
#define LIS3DH_TIME_LIMIT               0x3B
#define LIS3DH_TIME_LATENCY             0x3C
#define LIS3DH_TIME_WINDOW              0x3D

#define LIS3DH_WHO_AM_I_VAL             0b00110011
#define LIS3DH_CTRL_REG0_VAL            0b00010000
#define LIS3DH_TEMP_CFG_REG_VAL         0b00000000
#define LIS3DH_CTRL_REG1_VAL            0b00001111
#define LIS3DH_CTRL_REG2_VAL            0b11001001
#define LIS3DH_CTRL_REG3_VAL            0b01000100
#define LIS3DH_CTRL_REG4_VAL            0b00000000
#define LIS3DH_CTRL_REG5_VAL            0b01001010
#define LIS3DH_CTRL_REG6_VAL            0b00100000
#define LIS3DH_FIFO_CTRL_REG_VAL        0b00000000
#define LIS3DH_INT1_CFG_VAL             0b00101010
//#define LIS3DH_INT1_THS_VAL             0b00001000
#define LIS3DH_INT1_DURATION_VAL        0b00000000
#define LIS3DH_INT2_CFG_VAL             0b10010101
//#define LIS3DH_INT2_THS_VAL             0b00001000
#define LIS3DH_INT2_DURATION_VAL        0b00000000

#define LIS3DH_REG_BITMASK              0b00111111
#define LIS3DH_FSS_BITMASK              0b00011111
#define LIS3DH_INT1_SRC_IA_BITMASK      0b01000000

#define LIS3DH_READ_REQ_BIT             0x80
#define LIS3DH_AUTO_INCREMENT_BIT       0x40

#define LIS3DH_IS_READ_BIT_SET(x)       ((x) & LIS3DH_READ_REQ_BIT)
#define LIS3DH_GET_REG(x)               ((x) & LIS3DH_REG_BITMASK)

#define ACC_RX_BUFFER_SIZE              (197)
#define ACC_TX_BUFFER_SIZE              (8)
#define ACC_DATA_BUFFER_LEN             (12)
#define ACC_COMMANDS_MAX                (25)
#define ACC_READ_BUF_SIZE               (ACC_RX_BUFFER_SIZE)

#define ACC_CMD_W_INIT(r, v)            (acc_command_t){.base.command = command_execute, .rx_len = 0, .reg = r, .tx_value = v, .read = false}
#define ACC_CMD_R_INIT(r, l)            (acc_command_t){.base.command = command_execute, .rx_len = l, .reg = r, .tx_value = 0, .read = true}


typedef enum
{
    ACC_STATE_IDLE,
    ACC_STATE_WAIT_FOR_DATA,
} acc_state_t;

typedef struct
{
    command_t   base;
    uint16_t    rx_len;
    uint8_t     reg;
    uint8_t     tx_value;
    bool        read;
} acc_command_t;


static void event_handler(winsens_event_t event);
static void dio_callback(digital_io_input_pin_t pin, bool on);
static void hpf_task(void *p_data, uint16_t data_size);
static void ff_task(void *p_data, uint16_t data_size);

static void lis3dh_init(void);
static uint8_t get_freq_cfg(void);
static uint8_t get_hp_threshold_cfg(void);
static uint8_t get_ff_threshold_cfg(void);
static void write_reg(uint8_t reg, uint8_t value);
static void read_reg(uint8_t reg);
static void read_multiple_bytes(uint8_t reg, uint16_t len);
static void store_data(uint8_t* data, uint16_t size);
static void update_subscribers(void);

static void update_ff_threshold(void);
static void update_hp_threshold(void);

static void command_execute(command_t const* command);
static bool execute_one_cmd(void);
static bool execute_one_cmd_force(void);


static bool                     g_initialized                           = false;
static acc_state_t              g_current_state                         = ACC_STATE_IDLE;
static uint8_t                  g_fss                                   = 0; // fifo stored samples
static uint16_t                 g_ff_threshold                          = ACC_CFG_FREEFALL_THRESHOLD;
static uint16_t                 g_hp_threshold                          = ACC_CFG_HIPASS_THRESHOLD;

static subscribers_t            g_subscribers                           = { NULL, 0 };
static uint8_t                  g_rx_buffer[ACC_RX_BUFFER_SIZE];
static uint8_t                  g_tx_buffer[ACC_TX_BUFFER_SIZE];
static circular_buf_t           g_cmd_buf;
static acc_command_t            g_cmd_raw_buf[ACC_COMMANDS_MAX];
static circular_buf_t           g_read_buf;
static acc_command_t            g_read_raw_buf[ACC_READ_BUF_SIZE];
static winsens_event_handler_t  g_evt_handlers[ACC_CFG_SUBSCRIBERS_NUM] = { NULL };


LOG_REGISTER();


winsens_status_t acc_init(void)
{
    if (!g_initialized)
    {
        g_initialized = true;

        critical_region_init();

        circular_buf_safe_init(&g_cmd_buf, (uint8_t*)g_cmd_raw_buf, sizeof(g_cmd_raw_buf));
        circular_buf_safe_init(&g_read_buf, (uint8_t*)g_read_raw_buf, sizeof(g_read_raw_buf));

        spi_init();
        spi_subscribe(SPI_CFG_ACC, event_handler);

        digital_io_init();
        digital_io_register_callback(DIGITAL_IO_INPUT_ACC_INT_1, dio_callback);
        digital_io_register_callback(DIGITAL_IO_INPUT_ACC_INT_2, dio_callback);

        subscribers_init(&g_subscribers, g_evt_handlers, ACC_CFG_SUBSCRIBERS_NUM);

        lis3dh_init();
    }

    return WINSENS_OK;
}

winsens_status_t acc_subscribe(winsens_event_handler_t event_handler)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    return subscribers_add(&g_subscribers, event_handler);
}

winsens_status_t acc_get_data(acc_t* data, uint16_t len)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    circular_buf_pop(&g_read_buf, (uint8_t*)data, len * sizeof(acc_t));
    return WINSENS_OK;
}

uint16_t acc_get_data_len(void)
{
    return circular_buf_size(&g_read_buf) / sizeof(acc_t);
}

void acc_set_ff_threshold(uint16_t threshold)
{
    if (threshold != g_ff_threshold)
    {
        g_ff_threshold = threshold;
        update_ff_threshold();

        winsens_event_t e = { .id = ACC_EVT_FREEFALL_THRESHOLD_CHANGE, .data = threshold };
        subscribers_update(&g_subscribers, e);
    }
}

uint16_t acc_get_ff_threshold(void)
{
    return g_ff_threshold;
}

void acc_set_hp_threshold(uint16_t threshold)
{
    if (threshold != g_hp_threshold)
    {
        g_hp_threshold = threshold;
        update_hp_threshold();

        winsens_event_t e = { .id = ACC_EVT_HIPASS_THRESHOLD_CHANGE, .data = threshold };
        subscribers_update(&g_subscribers, e);
    }
}

uint16_t acc_get_hp_threshold(void)
{
    return g_hp_threshold;
}

static void event_handler(winsens_event_t event)
{
    if (SPI_EVT_TRANSFER_DONE == event.id)
    {
        switch (LIS3DH_GET_REG(g_tx_buffer[0]))
        {
            case LIS3DH_FIFO_SRC_REG:
            {
                g_fss = g_rx_buffer[1] & LIS3DH_FSS_BITMASK;
                if (ACC_CFG_FIFO_SAMPLES_NUM < g_fss)
                {
                    acc_command_t cmd = ACC_CMD_R_INIT(LIS3DH_OUT_X_L, g_fss * sizeof(acc_t));
                    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));
                }
                break;
            }

            case LIS3DH_INT1_SRC:
            {
                uint8_t ia = g_rx_buffer[1] & LIS3DH_INT1_SRC_IA_BITMASK;

                if (0 != ia)
                {
                    winsens_event_t e = { .id = ACC_EVT_HIPASS_INT, .data = 0 };
                    subscribers_update(&g_subscribers, e);

                    task_queue_add(&g_rx_buffer[1], 1, hpf_task);
                }
                break;
            }

            case LIS3DH_INT2_SRC:
            {
                uint8_t ia = g_rx_buffer[1] & LIS3DH_INT1_SRC_IA_BITMASK;

                if (0 != ia)
                {
                    winsens_event_t e = { .id = ACC_EVT_FREEFALL_INT, .data = 0 };
                    subscribers_update(&g_subscribers, e);

                    task_queue_add(&g_rx_buffer[1], 1, ff_task);
                }
                break;
            }

            case LIS3DH_OUT_X_L:
            {
                store_data(&g_rx_buffer[1], g_fss * sizeof(acc_t));
                update_subscribers();
                break;
            }

            case LIS3DH_CTRL_REG0:
            {
                if (LIS3DH_CTRL_REG0_VAL != g_rx_buffer[1])
                {
                    LOG_ERROR("LIS3DH_CTRL_REG0 read failed, value: 0x%02x", g_rx_buffer[1]);
                }
                break;
            }

            case LIS3DH_WHO_AM_I:
            {
                if (LIS3DH_WHO_AM_I_VAL != g_rx_buffer[1])
                {
                    LOG_ERROR("LIS3DH_WHO_AM_I read failed, value: 0x%02x", g_rx_buffer[1]);
                }
                break;
            }

            default:
                break;
        }
    }

    execute_one_cmd_force();
}

static void dio_callback(digital_io_input_pin_t pin, bool on)
{
    if (DIGITAL_IO_INPUT_ACC_INT_1 == pin && on)
    {
        acc_command_t cmd = ACC_CMD_R_INIT(LIS3DH_FIFO_SRC_REG, 1);
        circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

        cmd = ACC_CMD_R_INIT(LIS3DH_INT1_SRC, 1);
        circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

        execute_one_cmd();
    }
    else if (DIGITAL_IO_INPUT_ACC_INT_2 == pin && on)
    {
        acc_command_t cmd = ACC_CMD_R_INIT(LIS3DH_INT2_SRC, 1);
        circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

        execute_one_cmd();
    }
}

static void hpf_task(void *p_data, uint16_t data_size)
{
//    LOG_DEBUG("xxx xxx xxx HP 0x%02x INT 0x%x xxx xxx xxx", get_hp_threshold_cfg(), *(uint8_t*)p_data);
}

static void ff_task(void *p_data, uint16_t data_size)
{
//    LOG_DEBUG("xxx xxx xxx FF 0x%02x INT 0x%x xxx xxx xxx", get_ff_threshold_cfg(), *(uint8_t*)p_data);
}

static void lis3dh_init(void)
{
    acc_command_t cmd = ACC_CMD_R_INIT(LIS3DH_WHO_AM_I, 1);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_R_INIT(LIS3DH_CTRL_REG0, 1);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));


    // Set config registers
    cmd = ACC_CMD_W_INIT(LIS3DH_CTRL_REG1, LIS3DH_CTRL_REG1_VAL | get_freq_cfg());
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_W_INIT(LIS3DH_CTRL_REG2, LIS3DH_CTRL_REG2_VAL);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_W_INIT(LIS3DH_CTRL_REG3, LIS3DH_CTRL_REG3_VAL);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_W_INIT(LIS3DH_CTRL_REG4, LIS3DH_CTRL_REG4_VAL);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_W_INIT(LIS3DH_CTRL_REG5, LIS3DH_CTRL_REG5_VAL);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_W_INIT(LIS3DH_CTRL_REG6, LIS3DH_CTRL_REG6_VAL);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));


    // Set fifo registers
    cmd = ACC_CMD_W_INIT(LIS3DH_FIFO_CTRL_REG, LIS3DH_FIFO_CTRL_REG_VAL | ACC_CFG_FIFO_SAMPLES_NUM);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));


    // Set int 1 registers
    cmd = ACC_CMD_W_INIT(LIS3DH_INT1_THS, get_hp_threshold_cfg());
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_W_INIT(LIS3DH_INT1_CFG, LIS3DH_INT1_CFG_VAL);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_W_INIT(LIS3DH_INT1_DURATION, LIS3DH_INT1_DURATION_VAL);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));


    // Set int 2 registers
    cmd = ACC_CMD_W_INIT(LIS3DH_INT2_THS, get_ff_threshold_cfg());
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_W_INIT(LIS3DH_INT2_CFG, LIS3DH_INT2_CFG_VAL);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_W_INIT(LIS3DH_INT2_DURATION, LIS3DH_INT2_DURATION_VAL);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));


    // Read status registers
    cmd = ACC_CMD_R_INIT(LIS3DH_INT1_SRC, 1);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_R_INIT(LIS3DH_INT2_SRC, 1);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_R_INIT(LIS3DH_FIFO_SRC_REG, 1);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    cmd = ACC_CMD_R_INIT(LIS3DH_REFERENCE, 1);
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));

    execute_one_cmd();
}

static uint8_t get_freq_cfg(void)
{
    uint8_t cfg = 0;
    switch(ACC_CFG_SAMPLE_FREQ)
    {
        case 1:
            cfg = (0x01 << 4);
        break;
        case 10:
        default:
            cfg = (0x02 << 4);
        break;
        case 25:
            cfg = (0x03 << 4);
        break;
        case 50:
            cfg = (0x04 << 4);
        break;
        case 100:
            cfg = (0x05 << 4);
        break;
        case 200:
            cfg = (0x06 << 4);
        break;
        case 400:
            cfg = (0x07 << 4);
        break;
        case 1600:
            cfg = (0x08 << 4);
        break;
        case 5000:
            cfg = (0x09 << 4);
        break;
    }

    return cfg;
}

static uint8_t get_hp_threshold_cfg(void)
{
    uint8_t lsb_val = 0;
    switch(ACC_CFG_RANGE)
    {
        case 2:
        default:
            lsb_val = 16;
        break;
        case 4:
            lsb_val = 32;
        break;
        case 8:
            lsb_val = 62;
        break;
        case 16:
            lsb_val = 186;
        break;
    }

    uint16_t reg_val = g_hp_threshold / lsb_val;

    if (0x7F < reg_val)
    {
        reg_val = 0x7F;
    }

    return (uint8_t)reg_val;
}

static uint8_t get_ff_threshold_cfg(void)
{
    uint8_t lsb_val = 0;
    switch(ACC_CFG_RANGE)
    {
        case 2:
        default:
            lsb_val = 16;
        break;
        case 4:
            lsb_val = 32;
        break;
        case 8:
            lsb_val = 62;
        break;
        case 16:
            lsb_val = 186;
        break;
    }

    uint16_t reg_val = g_ff_threshold / lsb_val;

    if (0x7F < reg_val)
    {
        reg_val = 0x7F;
    }

    return (uint8_t)reg_val;
}

static void write_reg(uint8_t reg, uint8_t value)
{
    g_tx_buffer[0] = reg;
    g_tx_buffer[1] = value;
    spi_transfer(SPI_CFG_ACC, g_tx_buffer, 2, g_rx_buffer, 0);
}

static void read_reg(uint8_t reg)
{
    g_tx_buffer[0] = reg | LIS3DH_READ_REQ_BIT;
    spi_transfer(SPI_CFG_ACC, g_tx_buffer, 1, g_rx_buffer, 2);
}

static void read_multiple_bytes(uint8_t reg, uint16_t len)
{
    g_tx_buffer[0] = reg | LIS3DH_READ_REQ_BIT | LIS3DH_AUTO_INCREMENT_BIT;
    spi_transfer(SPI_CFG_ACC, g_tx_buffer, 1, g_rx_buffer, len + 1);
}

static void store_data(uint8_t* data, uint16_t size)
{
    circular_buf_push(&g_read_buf, data, size);
}

static void update_subscribers(void)
{
    const uint32_t len = (circular_buf_size(&g_read_buf) / sizeof(acc_t));
    if (len)
    {
        winsens_event_t e = { .id = ACC_EVT_NEW_DATA, .data = len };
        subscribers_update(&g_subscribers, e);
    }
}

static void update_ff_threshold(void)
{
    acc_command_t cmd = ACC_CMD_W_INIT(LIS3DH_INT2_THS, get_ff_threshold_cfg());
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));
    execute_one_cmd();
}

static void update_hp_threshold(void)
{
    acc_command_t cmd = ACC_CMD_W_INIT(LIS3DH_INT1_THS, get_hp_threshold_cfg());
    circular_buf_push(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));
    execute_one_cmd();
}

static void command_execute(command_t const* command)
{
    acc_command_t const* acc_cmd = (acc_command_t*)command;

    if (acc_cmd->read)
    {
        if (1 == acc_cmd->rx_len)
        {
            read_reg(acc_cmd->reg);
        }
        else
        {
            read_multiple_bytes(acc_cmd->reg, acc_cmd->rx_len);
        }
    }
    else
    {
        write_reg(acc_cmd->reg, acc_cmd->tx_value);
    }
}

static bool execute_one_cmd(void)
{
    bool executed = false;

    CRITICAL_REGION_IN();

    if (ACC_STATE_IDLE == g_current_state)
    {
        if (circular_buf_size(&g_cmd_buf) > 0)
        {
            executed = true;
            g_current_state = ACC_STATE_WAIT_FOR_DATA;

            acc_command_t cmd;
            circular_buf_pop(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));
            cmd.base.command(&cmd.base);
        }
        else
        {
            g_current_state = ACC_STATE_IDLE;
        }
    }

    CRITICAL_REGION_OUT();

    return executed;
}

static bool execute_one_cmd_force(void)
{
    bool executed = false;

    CRITICAL_REGION_IN();

    if (circular_buf_size(&g_cmd_buf) > 0)
    {
        executed = true;
        g_current_state = ACC_STATE_WAIT_FOR_DATA;

        acc_command_t cmd;
        circular_buf_pop(&g_cmd_buf, (uint8_t*)&cmd, sizeof(acc_command_t));
        cmd.base.command(&cmd.base);
    }
    else
    {
        g_current_state = ACC_STATE_IDLE;
    }

    CRITICAL_REGION_OUT();

    return executed;
}

