/*
 * pwr_mgr_cfg.h
 *
 *  Created on: 14.10.2021
 *      Author: Damian Plonek
 */

#ifndef PWR_MGR_CFG_H_
#define PWR_MGR_CFG_H_

#include "winsens.h"
#include "digital_io_cfg.h"

#define PWR_MGR_CFG_SUBSCRIBERS_NUM     (2)
#define PWR_MGR_CFG_ACTIVITY_MAP_INIT   { \
    { WINSENS_EVT_IDLE, PWR_MGR_ACTIVITY_GOTO_SLEEP } \
}

#define PWR_MGR_CFG_WAKEUP_PINS         { \
    DIGITAL_IO_INPUT_ACC_INT_HPF, \
    DIGITAL_IO_INPUT_ACC_INT_FF, \
}


#endif /* PWR_MGR_CFG_H_ */
