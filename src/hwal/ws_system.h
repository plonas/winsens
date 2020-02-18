/*
 * ws_system.h
 *
 *  Created on: 27.12.2019
 *      Author: Damian Plonek
 */

#ifndef WS_SYSTEM_H_
#define WS_SYSTEM_H_

#include "winsens_types.h"

WINSENS_Status_e WS_SystemInit(void);
void WS_SystemDeinit(void);

uint32_t WS_SystemGetTime(void);

#endif /* WS_SYSTEM_H_ */
