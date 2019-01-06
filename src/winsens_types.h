/*
 * winsens_types.h
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#ifndef WINSENS_TYPES_H_
#define WINSENS_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum
{
    WINSENS_OK,
    WINSENS_ERROR,
    WINSENS_NO_RESOURCES,

} WINSENS_Status_e;

typedef enum
{
    WINSENS_TOPIC_WINDOW_STATE,
    WINSENS_TOPIC_BATTERY,

} WINSENS_Topic_e;

#endif /* WINSENS_TYPES_H_ */
