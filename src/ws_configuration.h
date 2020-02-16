/*
 * configuration.h
 *
 *  Created on: 13.05.2019
 *      Author: Damian Plonek
 */

#ifndef WS_CONFIGURATION_H_
#define WS_CONFIGURATION_H_

#include "winsens_types.h"

#define WS_CONFIGURATION_ADDR_LEN       6

typedef struct
{
    bool windowEnabled[WS_WINDOWS_NUMBER];
    uint16_t windowThreshold[WS_WINDOWS_NUMBER];
    bool bonded;
    uint8_t address[WS_CONFIGURATION_ADDR_LEN];

} WS_Configuration_t;

WINSENS_Status_e WS_ConfigurationInit(void);

const WS_Configuration_t * WS_ConfigurationGet(void);

#endif /* WS_CONFIGURATION_H_ */
