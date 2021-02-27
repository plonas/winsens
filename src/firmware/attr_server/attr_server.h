/*
 * attr_server.h
 *
 *  Created on: 27.02.2021
 *      Author: Damian Plonek
 */

#ifndef ATTR_SERVER_H_
#define ATTR_SERVER_H_


#include "winsens_types.h"

typedef struct
{
    uint16_t    value_len;
    uint8_t     *value;
} attr_server_value_t;

typedef uint32_t attr_server_attr_id_t;
typedef void (*attr_server_cb_t) (attr_server_attr_id_t attr_id, attr_server_value_t value);


winsens_status_t attr_server_init(void);

winsens_status_t attr_server_subscribe(attr_server_cb_t callback);

winsens_status_t attr_server_update(attr_server_attr_id_t attr_id, attr_server_value_t value);


#endif /* ATTR_SERVER_H_ */
