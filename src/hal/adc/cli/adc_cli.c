/*
 * adc_cli.c
 *
 *  Created on: 09.11.2022
 *      Author: Damian.Plonek
 */

#include "cli.h"
#include "adc.h"
#include "adc_cli.h"
#include "utils.h"

/*
 ******************************************************************************
 * Function prototypes
 ******************************************************************************
 */
static void adc_cli(const cli_t *cli, size_t argc, char **argv);
static void adc_get_voltage_cli(const cli_t *cli, size_t argc, char **argv);

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */

/*
 ******************************************************************************
 * Interface functions
 ******************************************************************************
 */

/*
 ******************************************************************************
 * Private functions
 ******************************************************************************
 */
static void adc_cli(const cli_t *cli, size_t argc, char **argv)
{
    UTILS_UNUSED_VAR(argv);

    if (1 == argc)
    {
        cli_help_print();
    }

    (void)adc_get_voltage_cli;
}

static void adc_get_voltage_cli(const cli_t *cli, size_t argc, char **argv)
{
    if (1 == argc)
    {
        cli_help_print();
    }

    if (2 != argc)
    {
        cli_error("%s: bad parameter count", argv[0]);
        return;
    }

    cli_arg_t val;

    if (!cli_str_to_val(argv[1], CLI_ARG_TYPE_INT_16, &val))
    {
        cli_error("%s: wrong number", argv[0]);
        return;
    }

    int16_t voltage = adc_get_voltage(val.num_int16);

    cli_print("voltage: %d", voltage);
}

CLI_CREATE_STATIC_SUBCMD_SET(m_sub_adc)
{
    CLI_CMD(get_voltage,  NULL, "Convert to a voltage value.",  adc_get_voltage_cli),
    CLI_SUBCMD_SET_END
};
CLI_CMD_REG(adc, &m_sub_adc, "get voltage", adc_cli);
// CLI_CMD_REG(adc, NULL, "get voltage", adc_cli);