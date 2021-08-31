/*
 * system.h
 *
 *  Created on: 27.12.2019
 *      Author: Damian Plonek
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_


#include "winsens_types.h"

winsens_status_t system_init(void);

uint32_t system_get_time(void); // in 100ms units


#endif /* SYSTEM_H_ */
