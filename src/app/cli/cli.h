/*
 * cli.h
 *
 *  Created on: 09.11.2022
 *      Author: Damian.Plonek
 */

#ifndef CLI_H_
#define CLI_H_


#include "winsens_types.h"

#define CLI_ARGS_NUM_MAX    8

#define CLI_STR_ARG(_str)    {.len = 1, .args = {{.str = (_str), .type = CLI_ARG_TYPE_STR}}}

typedef enum
{
    CLI_ARG_TYPE_UNDEFINED,
    CLI_ARG_TYPE_UINT_32,
    CLI_ARG_TYPE_UINT_16,
    CLI_ARG_TYPE_UINT_8,
    CLI_ARG_TYPE_INT_32,
    CLI_ARG_TYPE_INT_16,
    CLI_ARG_TYPE_INT_8,
    CLI_ARG_TYPE_STR,
} cli_arg_type_t;

typedef struct
{
    cli_arg_type_t type;
    union
    {
        uint32_t num_uint32;
        uint16_t num_uint16;
        uint8_t num_uint8;
        int32_t num_int32;
        int16_t num_int16;
        int8_t num_int8;
        const char *str;
    };
} cli_arg_t;

typedef struct cli_args_list_t
{
    uint8_t len;
    cli_arg_t args[CLI_ARGS_NUM_MAX];
} cli_args_list_t;

typedef struct cli_ret_args_list_t
{
    uint8_t len;
    cli_arg_t args[1];
} cli_ret_args_list_t;

typedef void (*cli_invoke_fn)(const cli_args_list_t *args);

typedef struct cli_invoke_info_t
{
    cli_invoke_fn invoke_fn;
    cli_arg_type_t args_types[];
} cli_invoke_info_t;


winsens_status_t cli_init(void);
winsens_status_t cli_register_cmd(const char *cmd, const char *help, const cli_invoke_info_t *invoke);
void cli_answer(bool result, const cli_ret_args_list_t *answer);


#endif /* CLI_H_ */
