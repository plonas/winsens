/*
 * winsens_ids.h
 *
 *  Created on: 30.07.2020
 *      Author: Damian Plonek
 */

#ifndef WINSENS_IDS_H_
#define WINSENS_IDS_H_

#include <stdint.h>


typedef enum
{
    WINSENS_IDS_MODULE_BUTTON = 0x0001,
    WINSENS_IDS_MODULE_SERVER,

} winsens_ids_module_enum_t;

typedef uint16_t winsens_ids_module_t;


#endif /* WINSENS_IDS_H_ */
