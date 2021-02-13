/*
 * config.h
 *
 *  Created on: 13.05.2019
 *      Author: Damian Plonek
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "winsens_types.h"
#include "window_state_cfg.h"

#define CONFIGURATION_ADDR_LEN          6

typedef struct
{
    bool        windowEnabled[WINDOW_STATE_CFG_WINDOWS_NUMBER];
    uint16_t    windowThreshold[WINDOW_STATE_CFG_WINDOWS_NUMBER];
    bool        bonded;
    uint8_t     address[CONFIGURATION_ADDR_LEN];

} config_t;

winsens_status_t config_init(void);

const config_t * config_get(void);

winsens_status_t config_set(
    const config_t *configuration);

#endif /* CONFIG_H_ */
