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
    WS_WINDOW_STATE_UNKNOWN,
    WS_WINDOW_STATE_OPEN,
    WS_WINDOW_STATE_CLOSED

} WS_WindowState_e;

typedef enum
{
    WS_WINDOW_1,
    WS_WINDOW_2,
    WS_WINDOW_3,

    WS_WINDOWS_NUMBER
} WS_Window_e;

#endif /* WINSENS_TYPES_H_ */
