/*
 * winsens.h
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#ifndef WINSENS_H_
#define WINSENS_H_

#include "winsens_types.h"

WINSENS_Status_e WINSENS_Init();

void WINSENS_Deinit();

WINSENS_Status_e WINSENS_Loop();

#endif /* WINSENS_H_ */
