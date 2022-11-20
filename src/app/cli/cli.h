/*
 * cli.h
 *
 *  Created on: 09.11.2022
 *      Author: Damian.Plonek
 */

#ifndef CLI_H_
#define CLI_H_


#include "winsens_types.h"
#include "cli_internal.h"

#define CLI_CMD_REG                     CLI_INTERNAL_CMD_REG
#define CLI_CMD                         CLI_INTERNAL_CMD
#define CLI_CREATE_STATIC_SUBCMD_SET    CLI_INTERNAL_CREATE_STATIC_SUBCMD_SET
#define CLI_SUBCMD_SET_END              CLI_INTERNAL_SUBCMD_SET_END

typedef cli_internal_t cli_t;

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

#define cli_error(_fmt, ...)        cli_error_raw(_fmt "\n", ##__VA_ARGS__)
#define cli_warn(_fmt, ...)         cli_warn_raw(_fmt "\n", ##__VA_ARGS__)
#define cli_info(_fmt, ...)         cli_info_raw(_fmt "\n", ##__VA_ARGS__)
#define cli_print(_fmt, ...)        cli_print_raw(_fmt "\n", ##__VA_ARGS__)

winsens_status_t cli_init(void);
void cli_process(void);
bool cli_str_to_val(const char *str, cli_arg_type_t type, cli_arg_t *val);

void cli_help_print(void);
void cli_print_raw(char const *fmt, ...);
void cli_info_raw(char const *fmt, ...);
void cli_warn_raw(char const *fmt, ...);
void cli_error_raw(char const *fmt, ...);


#endif /* CLI_H_ */
