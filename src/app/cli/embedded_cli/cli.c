/*
 * cli.c
 *
 *  Created on: 12.11.2022
 *      Author: Damian.Plonek
 */

#include "cli.h"
#include "cli_cfg.h"
#include "embedded_cli.h"
#define ILOG_MODULE_NAME cli
#include "log.h"
#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"

#include <stdlib.h>


LOG_REGISTER();


#define CLI_CFG_INIT_LIST_LEN   (sizeof(client_init_list)/sizeof(client_init_list[0]))

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
static void write_char(EmbeddedCli *embedded_cli, char c);
static void cmd_handler(EmbeddedCli *cli, char *args, void *context);
static bool str_to_val(const char *str, cli_arg_type_t type, cli_arg_t *val);

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */
EmbeddedCli *embedded_cli = NULL;

/*
 ******************************************************************************
 * Interface functions
 ******************************************************************************
 */
winsens_status_t cli_init(void)
{
    const cli_client_init_fn client_init_list[] = CLI_CFG_INIT_LIST;
    EmbeddedCliConfig *config = embeddedCliDefaultConfig();
    config->maxBindingCount = 16;

    embedded_cli = embeddedCliNew(config);
    embedded_cli->writeChar = write_char;

    for (int i = 0; i < CLI_CFG_INIT_LIST_LEN; ++i)
    {
        if (client_init_list[i])
        {
            client_init_list[i]();
        }
    }

    return WINSENS_OK;
}

winsens_status_t cli_register_cmd(const char *cmd, const char *help, const cli_invoke_info_t *invoke)
{
    CliCommandBinding binding = {cmd, help, true, cmd_handler, (void *)invoke};
    embeddedCliAddBinding(embedded_cli, binding);
    return WINSENS_OK;
}

void cli_answer(bool result, const cli_ret_args_list_t *answer)
{
    //TODO cli_answer
}

/*
 ******************************************************************************
 * Private functions
 ******************************************************************************
 */
static void write_char(EmbeddedCli *embeddedCli, char c)
{
    //TODO write_char
}

static void cmd_handler(EmbeddedCli *cli, char *args, void *context)
{
    const cli_invoke_info_t *invoke_info = context;
    LOG_WARNING_BOOL_RETURN((NULL == invoke_info), ;);

    const cli_invoke_fn invoke_fn = invoke_info->invoke_fn;
    const cli_arg_type_t *types = invoke_info->args_types;
    cli_args_list_t arg_list = {0};

    if (invoke_fn &&
        types)
    {
        embeddedCliTokenizeArgs(args);
        const uint8_t len = embeddedCliGetTokenCount(args);

        for (uint8_t i = 0; i < len; ++i)
        {
            if (CLI_ARG_TYPE_UNDEFINED == types[i])
            {
                LOG_WARNING("Undefined type while converting");
                break;
            }

            const char *arg = embeddedCliGetToken(args, i);
            cli_arg_t val = {0};
            
            if (false == str_to_val(arg, types[i], &val))
            {
                break; //panic
            }

            arg_list.args[arg_list.len] = val;
            arg_list.len++;
        }

        invoke_fn(&arg_list);
    }
}

static bool str_to_val(const char *str, cli_arg_type_t type, cli_arg_t *val)
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