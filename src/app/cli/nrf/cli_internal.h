/*
 * cli_internal.h
 *
 *  Created on: 19.11.2022
 *      Author: Damian.Plonek
 */

#ifndef CLI_INTERNAL_H_
#define CLI_INTERNAL_H_


#include "nrf_cli.h"


#define CLI_INTERNAL_CMD_NRF_FN(_fn) \
    static void UTILS_CONCAT2(nrf_cli_, _fn) (nrf_cli_t const * p_cli, size_t argc, char **argv) \
    { \
        UTILS_UNUSED_VAR(p_cli); \
        _fn((uint8_t)argc, argv); \
    } \

/* #define CLI_INTERNAL_CMD_REG(_cmd, _subcmds, _help, _fn) \
    CLI_INTERNAL_CMD_NRF_FN(_fn) \
    NRF_CLI_CMD_REGISTER(_cmd, _subcmds, _help, UTILS_CONCAT2(nrf_cli_, _fn))
    */
#define CLI_INTERNAL_CMD_REG \
    NRF_CLI_CMD_REGISTER

#define CLI_INTERNAL_CMD \
    NRF_CLI_CMD

#define CLI_INTERNAL_CREATE_STATIC_SUBCMD_SET \
    NRF_CLI_CREATE_STATIC_SUBCMD_SET

#define CLI_INTERNAL_SUBCMD_SET_END \
    NRF_CLI_SUBCMD_SET_END

typedef nrf_cli_t cli_internal_t;


#endif /* CLI_INTERNAL_H_ */