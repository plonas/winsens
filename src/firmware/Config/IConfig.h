/*
 * IConfig.h
 *
 *  Created on: 13.05.2019
 *      Author: Damian Plonek
 */

#ifndef ICONFIG_H_
#define ICONFIG_H_

#include "winsens_types.h"
#include "IWindowStateCfg.h"

#define WS_CONFIGURATION_ADDR_LEN       6

typedef struct
{
    bool windowEnabled[IWINDOW_STATE_CFG_WINDOWS_NUMBER];
    uint16_t windowThreshold[IWINDOW_STATE_CFG_WINDOWS_NUMBER];
    bool bonded;
    uint8_t address[WS_CONFIGURATION_ADDR_LEN];

} WS_Configuration_t;

WINSENS_Status_e IConfig_Init(void);

const WS_Configuration_t * IConfig_Get(void);

WINSENS_Status_e IConfig_Set(
    const WS_Configuration_t *configuration);

#endif /* ICONFIG_H_ */
