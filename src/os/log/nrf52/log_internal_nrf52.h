/*
 * log_internal_nrf52.h
 *
 *  Created on: 28.11.2020
 *      Author: Damian Plonek
 */

#ifndef LOG_INTERNAL_NRF52_H_
#define LOG_INTERNAL_NRF52_H_

#include "nrf_error.h"
#include "log.h"

#define LOG_NRF_ERROR_CHECK(s)              do{ if (NRF_SUCCESS != s) { LOG_ERROR_INTERNAL(#s "(%u) != NRF_SUCCESS | %s:%u", s, __FUNCTION__, __LINE__) } }while(0)
#define LOG_NRF_WARNING_CHECK(s)            do{ if (NRF_SUCCESS != s) { LOG_WARNING_INTERNAL(#s "(%u) != NRF_SUCCESS | %s:%u", s, __FUNCTION__, __LINE__) } }while(0)
#define LOG_NRF_INFO_CHECK(s)               do{ if (NRF_SUCCESS != s) { LOG_INFO_INTERNAL(#s "(%u) != NRF_SUCCESS | %s:%u", s, __FUNCTION__, __LINE__) } }while(0)
#define LOG_NRF_DEBUG_CHECK(s)              do{ if (NRF_SUCCESS != s) { LOG_DEBUG_INTERNAL(#s "(%u) != NRF_SUCCESS | %s:%u", s, __FUNCTION__, __LINE__) } }while(0)

#define LOG_NRF_ERROR_RETURN(s, r)          do{ if (NRF_SUCCESS != s) { LOG_ERROR_INTERNAL(#s "(%u) != NRF_SUCCESS | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)
#define LOG_NRF_WARNING_RETURN(s, r)        do{ if (NRF_SUCCESS != s) { LOG_WARNING_INTERNAL(#s "(%u) != NRF_SUCCESS | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)
#define LOG_NRF_INFO_RETURN(s, r)           do{ if (NRF_SUCCESS != s) { LOG_INFO_INTERNAL(#s "(%u) != NRF_SUCCESS | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)
#define LOG_NRF_DEBUG_RETURN(s, r)          do{ if (NRF_SUCCESS != s) { LOG_DEBUG_INTERNAL(#s "(%u) != NRF_SUCCESS | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)


#endif /* LOG_INTERNAL_NRF52_H_ */
