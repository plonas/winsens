/*
 * winsens_ids.h
 *
 *  Created on: 30.07.2020
 *      Author: Damian Plonek
 */

#ifndef SRC_WINSENS_IDS_H_
#define SRC_WINSENS_IDS_H_

#include <stdint.h>


typedef enum
{
    WS_MODULE_ID_BUTTON = 0x0001,
    WS_MODULE_ID_SERVER,

} WS_ModuleId_e;

typedef uint16_t WS_ModuleId_t;


#endif /* SRC_WINSENS_IDS_H_ */
