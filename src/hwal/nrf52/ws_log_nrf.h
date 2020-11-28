/*
 * ws_log_nrf.h
 *
 *  Created on: 28.11.2020
 *      Author: Damian Plonek
 */

#ifndef WS_LOG_NRF_H_
#define WS_LOG_NRF_H_

#include "nrf_error.h"
#include "ws_log.h"

#define WS_LOG_NRF_ERROR_CHECK(s)           do{ if (NRF_SUCCESS != s) { WS_LOG_ERROR_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__) } }while(0)
#define WS_LOG_NRF_WARNING_CHECK(s)         do{ if (NRF_SUCCESS != s) { WS_LOG_WARNING_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__) } }while(0)
#define WS_LOG_NRF_INFO_CHECK(s)            do{ if (NRF_SUCCESS != s) { WS_LOG_INFO_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__) } }while(0)
#define WS_LOG_NRF_DEBUG_CHECK(s)           do{ if (NRF_SUCCESS != s) { WS_LOG_DEBUG_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__) } }while(0)

#define WS_NRF_ERROR_CHECK(s, r)            do{ if (NRF_SUCCESS != s) { WS_LOG_ERROR_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)
#define WS_NRF_WARNING_CHECK(s, r)          do{ if (NRF_SUCCESS != s) { WS_LOG_WARNING_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)
#define WS_NRF_INFO_CHECK(s, r)             do{ if (NRF_SUCCESS != s) { WS_LOG_INFO_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)
#define WS_NRF_DEBUG_CHECK(s, r)            do{ if (NRF_SUCCESS != s) { WS_LOG_DEBUG_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)


#endif /* WS_LOG_NRF_H_ */
