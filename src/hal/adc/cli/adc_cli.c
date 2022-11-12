/*
 * adc_cli.c
 *
 *  Created on: 09.11.2022
 *      Author: Damian.Plonek
 */

#include "cli.h"
#include "adc.h"
#include "adc_cli.h"

/*
 ******************************************************************************
 * Function prototypes
 ******************************************************************************
 */
static void adc_get_voltage_cli(const cli_args_list_t *args);

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */
static const cli_invoke_info_t g_invoke_info = {
    .invoke_fn = adc_get_voltage_cli,
    .args_types = {
        CLI_ARG_TYPE_INT_16,
        CLI_ARG_TYPE_UNDEFINED
    }
};

/*
 ******************************************************************************
 * Interface functions
 ******************************************************************************
 */
void adc_cli_init(void)
{
    cli_register_cmd("adc.get_voltage", "help", &g_invoke_info);
}

/*
 ******************************************************************************
 * Private functions
 ******************************************************************************
 */
static void adc_get_voltage_cli(const cli_args_list_t *args)
{
    if (NULL == args)
    {
        cli_ret_args_list_t no_args = CLI_STR_ARG("No Arguments");
        cli_answer(false, &no_args);
    }

    if (1 != args->len)
    {
        cli_ret_args_list_t wrong_args = CLI_STR_ARG("Wrong arguments");
        cli_answer(false, &wrong_args);
    }

    if (CLI_ARG_TYPE_INT_16 != args->args->type)
    {
        cli_ret_args_list_t wrong_type_args = CLI_STR_ARG("Wrong argument type");
        cli_answer(false, &wrong_type_args);
    }

    int16_t voltage = adc_get_voltage((int16_t)args->args->num_int16);

    cli_ret_args_list_t ret_args;
    ret_args.args->type = CLI_ARG_TYPE_INT_16;
    ret_args.args->num_int16 = voltage;
    ret_args.len = 1;
    
    cli_answer(true, &ret_args);
}