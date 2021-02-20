/*
 * server_defs.h
 *
 *  Created on: 20.02.2021
 *      Author: Damian Plonek
 */

#ifndef SERVER_DEFS_H_
#define SERVER_DEFS_H_

#include <stdbool.h>
#include <stdint.h>
#include "window_state_cfg.h"


typedef struct
{
    bool        bonded;
    uint16_t    window_state_threshold[WINDOW_STATE_CFG_NUMBER];

} server_config_t;

#endif /* SERVER_DEFS_H_ */
