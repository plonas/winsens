/*
 * ConfigStorage.h
 *
 *  Created on: 13.05.2019
 *      Author: Damian Plonek
 */

#ifndef CONFIG_STORAGE_H_
#define CONFIG_STORAGE_H_

#include "IConfig.h"

WINSENS_Status_e ConfigStorage_Init(void);

const WS_Configuration_t * ConfigStorage_Get(void);

WINSENS_Status_e ConfigStorage_Set(
    const WS_Configuration_t *configuration);

#endif /* CONFIG_STORAGE_H_ */
