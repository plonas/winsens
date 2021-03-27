/*
 * window_state_cfg.h
 *
 *  Created on: 10.01.2021
 *      Author: Damian.Plonek
 */

#ifndef WINDOW_STATE_CFG_H_
#define WINDOW_STATE_CFG_H_


typedef enum
{
    WINDOW_STATE_CFG_WINDOW_LEFT,
    WINDOW_STATE_CFG_WINDOW_RIGHT,

    WINDOW_STATE_CFG_NUMBER

} window_id_enum_t;

#define WINDOW_STATE_CFG_UPDATE_INTERVAL        1000 //sec.

#endif /* WINDOW_STATE_CFG_H_ */
