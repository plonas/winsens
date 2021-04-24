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
#include "command.h"
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
#define LIS3DH_CTRL_REG1_VAL            0b00001111
//#define LIS3DH_CTRL_REG2_VAL            0b00101111
#define LIS3DH_CTRL_REG3_VAL            0b00000100
#define LIS3DH_CTRL_REG4_VAL            0b00001000
#define LIS3DH_CTRL_REG5_VAL            0b01000000
//#define LIS3DH_CTRL_REG6_VAL            0b00000000
#define LIS3DH_FIFO_CTRL_REG_VAL        0b10001010

#define LIS3DH_REG_BITMASK              0b00111111
#define LIS3DH_FSS_BITMASK              0b00011111

#define LIS3DH_READ_REQ_BIT             0x80
#define LIS3DH_AUTO_INCREMENT_BIT       0x40

#define LIS3DH_IS_READ_BIT_SET(x)       ((x) & LIS3DH_READ_REQ_BIT)
#define LIS3DH_GET_REG(x)               ((x) & LIS3DH_REG_BITMASK)

#define ACC_RX_BUFFER_SIZE              (197)
#define ACC_TX_BUFFER_SIZE              (8)
#define ACC_DATA_BUFFER_LEN             (12)
#define ACC_COMMANDS_MAX                (8)

#define ACC_CMD_W_INIT(r, v)            (acc_command_t){.base.command = command_execute, .rx_len = 0, .reg = r, .tx_value = v, .read = false}
#define ACC_CMD_R_INIT(r, l)            (acc_command_t){.base.command = command_execute, .rx_len = l, .reg = r, .tx_value = 0, .read = true}


typedef enum
{
    ACC_STATE_IDLE,
    ACC_STATE_INIT,
    ACC_STATE_READ_FIFO,
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

static void change_state(acc_state_t state);

static void lis3dh_init(void);
static uint8_t get_freq_cfg(void);
static void write_reg(uint8_t reg, uint8_t value);
static void read_reg(uint8_t reg);
static void read_multiple_bytes(uint8_t reg, uint16_t len);
static void read_fifo(void);
static void handle_acc_data(uint8_t* data, uint16_t len);
static void update_subscribers(void);
static void command_execute(command_t const* command);
static bool execute_one_cmd(void);
static void reset_commands(void);


static bool                     g_initialized                           = false;
static acc_state_t              g_current_state                         = ACC_STATE_IDLE;
static uint8_t                  g_fss                                   = 0; // fifo stored samples
static acc_t                    g_hp_filter                             = {0};
static uint32_t                 g_skipped_samples                       = 0;

static winsens_event_handler_t  g_subscribers[ACC_CFG_MAX_SUBSCRIBERS]  = {NULL};
static uint8_t                  g_rx_buffer[ACC_RX_BUFFER_SIZE];
static uint8_t                  g_tx_buffer[ACC_TX_BUFFER_SIZE];
static acc_data_t               g_current_acc_data = {0};
static acc_command_t            g_commands[ACC_COMMANDS_MAX];
static uint8_t                  g_commands_num                           = 0;
static uint8_t                  g_commands_ran                           = 0;


LOG_REGISTER();


winsens_status_t acc_init(void)
{
    if (!g_initialized)
    {
        g_initialized = true;

        memset(g_commands, 0, sizeof(acc_command_t) * ACC_COMMANDS_MAX);
        g_commands_num = 0;
        g_commands_ran = 0;

        g_hp_filter = (acc_t){0, 0, 0};

        spi_init();
        spi_subscribe(event_handler);

        digital_io_register_callback(DIGITAL_IO_INPUT_ACC_INT, dio_callback);

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

winsens_status_t acc_set_high_pass(int16_t x, int16_t y, int16_t z)
{
    g_hp_filter.x = x;
    g_hp_filter.y = y;
    g_hp_filter.z = z;
    return WINSENS_OK;
}

winsens_status_t acc_get_data(acc_data_t* data)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    *data = g_current_acc_data;
    return WINSENS_OK;
}

static void event_handler(winsens_event_t event)
{
    if (SPI_EVT_TRANSFER_DONE == event.id)
    {
        LOG_DEBUG("SPI_EVT_TRANSFER_DONE: %d, 0x%x", g_current_state, LIS3DH_GET_REG(g_tx_buffer[0]));

        switch (g_current_state)
        {
            case ACC_STATE_INIT:
                if (false == execute_one_cmd())
                {
                    change_state(ACC_STATE_IDLE);
                }
                break;

            case ACC_STATE_READ_FIFO:
                switch (LIS3DH_GET_REG(g_tx_buffer[0]))
                {
                case LIS3DH_FIFO_SRC_REG:
                    g_fss = g_rx_buffer[1] & LIS3DH_FSS_BITMASK;
                    g_commands[g_commands_num++] = ACC_CMD_R_INIT(LIS3DH_OUT_X_L, g_fss * 6);
                    break;

                default:
                    break;
                }

                if (false == execute_one_cmd())
                {
                    change_state(ACC_STATE_IDLE);
                    handle_acc_data(&g_rx_buffer[1], g_fss * 6);
                }
                break;

            default:
                break;
        }
    }
}

static void dio_callback(digital_io_input_pin_t pin, bool on)
{
    if (DIGITAL_IO_INPUT_ACC_INT == pin && on)
    {
        read_fifo();
    }
}

static void change_state(acc_state_t state)
{
    // Exit actions
    switch (g_current_state)
    {
    case ACC_STATE_INIT:
        reset_commands();
        break;

    case ACC_STATE_READ_FIFO:
        reset_commands();
        break;

        default:
            break;
    }

    g_current_state = state;

    // Enter actions
    switch (g_current_state)
    {
        case ACC_STATE_INIT:
            g_commands[g_commands_num++] = ACC_CMD_W_INIT(LIS3DH_CTRL_REG1, LIS3DH_CTRL_REG1_VAL | get_freq_cfg());
            g_commands[g_commands_num++] = ACC_CMD_W_INIT(LIS3DH_CTRL_REG3, LIS3DH_CTRL_REG3_VAL);
            g_commands[g_commands_num++] = ACC_CMD_W_INIT(LIS3DH_CTRL_REG4, LIS3DH_CTRL_REG4_VAL);
            g_commands[g_commands_num++] = ACC_CMD_W_INIT(LIS3DH_CTRL_REG5, LIS3DH_CTRL_REG5_VAL);
            g_commands[g_commands_num++] = ACC_CMD_W_INIT(LIS3DH_FIFO_CTRL_REG, LIS3DH_FIFO_CTRL_REG_VAL);
            execute_one_cmd();
            break;

        case ACC_STATE_READ_FIFO:
            g_commands[g_commands_num++] = ACC_CMD_R_INIT(LIS3DH_FIFO_SRC_REG, 1);
            execute_one_cmd();
            break;

        default:
            break;
    }
}

static void lis3dh_init(void)
{
    change_state(ACC_STATE_INIT);
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
        default:
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

static void write_reg(uint8_t reg, uint8_t value)
{
    g_tx_buffer[0] = reg;
    g_tx_buffer[1] = value;
    spi_transfer(g_tx_buffer, 2, g_rx_buffer, 0);
}

static void read_reg(uint8_t reg)
{
    g_tx_buffer[0] = reg | LIS3DH_READ_REQ_BIT;
    spi_transfer(g_tx_buffer, 1, g_rx_buffer, 2);
}

static void read_multiple_bytes(uint8_t reg, uint16_t len)
{
    g_tx_buffer[0] = reg | LIS3DH_READ_REQ_BIT | LIS3DH_AUTO_INCREMENT_BIT;
    spi_transfer(g_tx_buffer, 1, g_rx_buffer, len + 1);
}

static void read_fifo(void)
{
    change_state(ACC_STATE_READ_FIFO);
}

static void handle_acc_data(uint8_t* data, uint16_t len)
{
    uint16_t i = 0;

    while (i < len)
    {
        g_current_acc_data.acc.x = *((int16_t*)&data[i]);
        i += 2;
        g_current_acc_data.acc.y = *((int16_t*)&data[i]);
        i += 2;
        g_current_acc_data.acc.z = *((int16_t*)&data[i]);
        i += 2;

        g_current_acc_data.time_delta = ACC_CFG_SAMPLE_FREQ * (1 + g_skipped_samples);

        if (g_current_acc_data.acc.x >= g_hp_filter.x ||
            g_current_acc_data.acc.y >= g_hp_filter.y ||
            g_current_acc_data.acc.z >= g_hp_filter.z)
        {
            g_skipped_samples = 0;
            update_subscribers();
        }
        else
        {
            g_skipped_samples++;
        }
    }
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

    LOG_DEBUG("Acc: %d %d %d", g_current_acc_data.acc.x, g_current_acc_data.acc.y, g_current_acc_data.acc.z);
    LOG_FLUSH();
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
    if (g_commands_ran < g_commands_num)
    {
        g_commands[g_commands_ran].base.command((command_t const*)&g_commands[g_commands_ran]);
        g_commands_ran++;
        return true;
    }

    return false;
}

static void reset_commands(void)
{
    g_commands_num = 0;
    g_commands_ran = 0;
}
