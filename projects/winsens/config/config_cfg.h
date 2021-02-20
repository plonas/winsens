/*
 * config_cfg.h
 *
 *  Created on: 20.02.2021
 *      Author: Damian Plonek
 */

#ifndef CONFIG_CFG_H_
#define CONFIG_CFG_H_


#include "server_defs.h"


typedef enum
{
    CONFIG_ID_SERVER,

} config_id_enum_t;

typedef struct
{
    server_config_t server_config;

} config_master_t;

#endif /* CONFIG_CFG_H_ */
