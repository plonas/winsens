/*
 * critical_region.h
 *
 *  Created on: 15.04.2021
 *      Author: Damian Plonek
 */

#ifndef CRITICAL_REGION_H_
#define CRITICAL_REGION_H_


#include "winsens_types.h"

#define CRITICAL_REGION_IN()            do{ critical_region_enter(); }while(0)
#define CRITICAL_REGION_OUT()           do{ critical_region_exit(); }while(0)


winsens_status_t critical_region_init(void);

winsens_status_t critical_region_enter(void);

winsens_status_t critical_region_exit(void);


#endif /* CRITICAL_REGION_H_ */
