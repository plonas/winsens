/*
 * acc_cfg.h
 *
 *  Created on: 16.04.2021
 *      Author: Damian Plonek
 */

#ifndef ACC_CFG_H_
#define ACC_CFG_H_


#define ACC_CFG_SAMPLE_FREQ         (10) // Hz

#define ACC_CFG_RANGE               (2) // +-[2, 4, 8, 16]mg (milli g-force)

#define ACC_CFG_HIPASS_THRESHOLD    (100) // mg (milli g-force)
#define ACC_CFG_FREEFALL_THRESHOLD  (350) // mg (milli g-force)

#define ACC_CFG_FIFO_SAMPLES_NUM    (1) // <= 32


#endif /* ACC_CFG_H_ */
