/*
 * ws_log.h
 *
 *  Created on: 29.12.2019
 *      Author: Damian Plonek
 */

#ifndef WS_LOG_H_
#define WS_LOG_H_


#include "ws_log_internal.h"

#define WS_LOG_INIT(timestamp_func)     WS_LOG_INIT_INTERNAL(timestamp_func)
#define WS_LOG_FLUSH()                  WS_LOG_FLUSH_INTERNAL()

#define WS_LOG_ERROR(...)               WS_LOG_ERROR_INTERNAL(__VA_ARGS__)
#define WS_LOG_WARNING(...)             WS_LOG_WARNING_INTERNAL(__VA_ARGS__)
#define WS_LOG_INFO(...)                WS_LOG_INFO_INTERNAL(__VA_ARGS__)
#define WS_LOG_DEBUG(...)               WS_LOG_DEBUG_INTERNAL(__VA_ARGS__)

#define WS_LOG_ERROR_CHECK(s)           WS_LOG_IF_ERROR(WINSENS_OK != (s), #s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__)
#define WS_LOG_WARNING_CHECK(s)         WS_LOG_IF_WARNING(WINSENS_OK != (s), #s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__)
#define WS_LOG_INFO_CHECK(s)            WS_LOG_IF_INFO(WINSENS_OK != (s), #s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__)
#define WS_LOG_DEBUG_CHECK(s)           WS_LOG_IF_DEBUG(WINSENS_OK != (s), #s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__)

#define WS_ERROR_CHECK(s, r)            do{ if (NRF_SUCCESS != s) { WS_LOG_ERROR_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)
#define WS_WARNING_CHECK(s, r)          do{ if (NRF_SUCCESS != s) { WS_LOG_WARNING_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)
#define WS_INFO_CHECK(s, r)             do{ if (NRF_SUCCESS != s) { WS_LOG_INFO_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)
#define WS_DEBUG_CHECK(s, r)            do{ if (NRF_SUCCESS != s) { WS_LOG_DEBUG_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)

#define WS_LOG_IF_ERROR(s, ...)         do{ if (s) { WS_LOG_ERROR_INTERNAL(__VA_ARGS__) } }while(0)
#define WS_LOG_IF_WARNING(s, ...)       do{ if (s) { WS_LOG_WARNING_INTERNAL(__VA_ARGS__) } }while(0)
#define WS_LOG_IF_INFO(s, ...)          do{ if (s) { WS_LOG_INFO_INTERNAL(__VA_ARGS__) } }while(0)
#define WS_LOG_IF_DEBUG(s, ...)         do{ if (s) { WS_LOG_DEBUG_INTERNAL(__VA_ARGS__) } }while(0)

#endif /* WS_LOG_H_ */
