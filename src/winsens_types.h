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

} winsens_status_t;

typedef uint32_t winsens_event_id_t;
typedef uint32_t winsens_event_data_t;

typedef struct
{
    union
    {
        winsens_event_id_t  id;
        struct
        {
            winsens_ids_module_t    moduleId;
            uint16_t                eventId;
        };
    };

    union
    {
        winsens_event_data_t    data;
        struct
        {
            uint16_t    data16a;
            uint16_t    data16b;
        };
        struct
        {
            uint8_t     data8a;
            uint8_t     data8b;
            uint8_t     data8c;
            uint8_t     data8d;
        };
    };

} winsens_event_t;

typedef void (*winsens_event_handler_t) (winsens_event_t event);

#endif /* WINSENS_TYPES_H_ */
