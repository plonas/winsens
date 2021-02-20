/*
 * config.h
 *
 *  Created on: 13.05.2019
 *      Author: Damian Plonek
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "winsens_types.h"


typedef uint16_t config_id_t;


winsens_status_t config_init(void);

void config_get(config_id_t id, void *config, uint16_t config_size, const void *default_config);

winsens_status_t config_set(config_id_t id, const void *config, uint16_t config_size);

#endif /* CONFIG_H_ */
