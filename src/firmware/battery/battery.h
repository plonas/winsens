/*
 * battery.h
 *
 *  Created on: 20.11.2021
 *      Author: Damian.Plonek
 */

#ifndef BATTERY_H_
#define BATTERY_H_


#include "winsens_types.h"


typedef uint8_t battery_level_t;


winsens_status_t battery_init(void);

winsens_status_t battery_subscribe(winsens_event_handler_t callback);

winsens_status_t battery_get_level(battery_level_t *level);


#endif /* BATTERY_H_ */
