/*
 * WinsensImpl.h
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#ifndef WINSENS_IMPL_H_
#define WINSENS_IMPL_H_

#include "IWinsens.h"

WINSENS_Status_e WinsensImpl_Init(
    WS_Server_t *server,
    const WS_Configuration_t *config);

void WinsensImpl_Deinit();

#endif /* WINSENS_IMPL_H_ */
