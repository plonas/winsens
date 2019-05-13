/*
 * configuration.h
 *
 *  Created on: 13.05.2019
 *      Author: Damian Plonek
 */

#ifndef WS_CONFIGURATION_H_
#define WS_CONFIGURATION_H_

#include "winsens_types.h"

typedef struct
{
    bool windowEnabled[WS_WINDOWS_NUMBER];

} WS_Configuration_t;

WINSENS_Status_e WS_ConfigurationInit(void);

const WS_Configuration_t * WS_ConfigurationGet(void);

#endif /* WS_CONFIGURATION_H_ */
