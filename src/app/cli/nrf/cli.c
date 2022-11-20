/*
 * cli.c
 *
 *  Created on: 13.11.2022
 *      Author: Damian.Plonek
 */

#include "cli.h"
#include "cli_cfg.h"
#define ILOG_MODULE_NAME cli
#include "log.h"
#include "log_internal_nrf52.h"

#include "nrf_cli.h"
#include "nrf_cli_uart.h"

#include <stdlib.h>


LOG_REGISTER();


#define CLI_LOG_QUEUE_SIZE      (16)
#define CLI_INIT_LIST_LEN       (sizeof(client_init_list)/sizeof(client_init_list[0]))

NRF_CLI_UART_DEF(m_cli_uart_transport, 0, 1024, 1024);
NRF_CLI_DEF(m_cli_uart,
            "ws:~$ ",
            &m_cli_uart_transport.transport,
            '\r',
            CLI_LOG_QUEUE_SIZE);

/*
 ******************************************************************************
 * Type definitions
 ******************************************************************************
 */
typedef void (*cli_client_init_fn)(void);

/*
 ******************************************************************************
 * Function prototypes
 ******************************************************************************
 */

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */
static bool m_initialized = false;

/*
 ******************************************************************************
 * Interface functions
 ******************************************************************************
 */
winsens_status_t cli_init(void)
{
    if (m_initialized)
    {
        return WINSENS_OK;
    }
    m_initialized = true;

    ret_code_t ret;

    nrf_drv_uart_config_t uart_config = NRF_DRV_UART_DEFAULT_CONFIG;
    uart_config.pseltxd = (((1) << 5) | ((13) & 0x1F));
    uart_config.pselrxd = (((1) << 5) | ((10) & 0x1F));
    uart_config.hwfc    = NRF_UART_HWFC_DISABLED;

    ret = nrf_cli_init(&m_cli_uart, &uart_config, true, true, NRF_LOG_SEVERITY_INFO);
    LOG_NRF_ERROR_CHECK(ret);

    ret = nrf_cli_start(&m_cli_uart);
    LOG_NRF_ERROR_CHECK(ret);

    return WINSENS_OK;
}

void cli_process(void)
{
    nrf_cli_process(&m_cli_uart);
}

bool cli_str_to_val(const char *str, cli_arg_type_t type, cli_arg_t *val)
{
    LOG_WARNING_BOOL_RETURN((NULL != str), false);
    LOG_WARNING_BOOL_RETURN((NULL != val), false);

    char *end = NULL;

    switch (type)
    {
        case CLI_ARG_TYPE_STR:
            val->str = str;
            break;

        case CLI_ARG_TYPE_INT_16:
            val->num_int16 = strtol(str, &end, 0);
            break;

        default:
            LOG_WARNING("Wrong type while converting");
            return false;
    }

    if (NULL != end && '\0' != *end)
    {
        LOG_WARNING("Convarting arg str failed");
        return false;
    }

    val->type = type;

    return true;
}

void cli_help_print(void)
{
    nrf_cli_help_print(&m_cli_uart, NULL, 0);
}

void cli_print_raw(char const *fmt, ...)
{
    va_list args = {0};
    va_start(args, fmt);

    nrf_cli_fprintf(&m_cli_uart, NRF_CLI_DEFAULT, fmt, &args);

    va_end(args);
}

void cli_info_raw(char const *fmt, ...)
{
    va_list args = {0};
    va_start(args, fmt);

    nrf_cli_fprintf(&m_cli_uart, NRF_CLI_INFO, fmt, &args);

    va_end(args);
}

void cli_warn_raw(char const *fmt, ...)
{
    va_list args = {0};
    va_start(args, fmt);

    nrf_cli_fprintf(&m_cli_uart, NRF_CLI_WARNING, fmt, &args);

    va_end(args);
}

void cli_error_raw(char const *fmt, ...)
{
    va_list args = {0};
    va_start(args, fmt);

    nrf_cli_fprintf(&m_cli_uart, NRF_CLI_ERROR, fmt, &args);

    va_end(args);
}

/*
 ******************************************************************************
 * Private functions
 ******************************************************************************
 */