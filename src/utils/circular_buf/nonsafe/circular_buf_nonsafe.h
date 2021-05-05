/*
 * circular_buf_nonsafe.h
 *
 *  Created on: 05.05.2021
 *      Author: Damian Plonek
 */

#ifndef CIRCULAR_BUF_NONSAFE_H_
#define CIRCULAR_BUF_NONSAFE_H_


#include "circular_buf.h"


winsens_status_t circular_buf_nonsafe_init(circular_buf_t* buf, uint8_t* data_buffer, uint32_t size);


#endif /* CIRCULAR_BUF_NONSAFE_H_ */
