/*
 * IWinsens.h
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#ifndef IWINSENS_H_
#define IWINSENS_H_

#include "winsens_types.h"
#include "IConfig.h"

WINSENS_Status_e IWinsens_Init(
    const WS_Configuration_t *config);

void IWinsens_Deinit();

#endif /* IWINSENS_H_ */
