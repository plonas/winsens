/*
 * winsens_types.h
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#ifndef WINSENS_TYPES_H_
#define WINSENS_TYPES_H_

#include "winsens_ids.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum
{
    WINSENS_OK,
    WINSENS_ERROR,
    WINSENS_BUSY,
    WINSENS_NOT_FOUND,
    WINSENS_NO_RESOURCES,
    WINSENS_INVALID_PARAMS,

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

    WS_WINDOWS_NUMBER
} WS_Window_e;

typedef uint32_t WS_EventId_t;
typedef uint32_t WS_EventData_t;

typedef struct
{
    union
    {
        WS_EventId_t        id;
        struct
        {
            WS_ModuleId_t   moduleId;
            uint16_t        eventId;
        };
    };

    union
    {
        WS_EventData_t      data;
        struct
        {
            uint16_t        data16a;
            uint16_t        data16b;
        };
        struct
        {
            uint8_t         data8a;
            uint8_t         data8b;
            uint8_t         data8c;
            uint8_t         data8d;
        };
    };

} WS_Event_t;

typedef void (*WS_EventHandler_f) (WS_Event_t event);

#endif /* WINSENS_TYPES_H_ */
