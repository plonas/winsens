/*
 * log.h
 *
 *  Created on: 29.12.2019
 *      Author: Damian Plonek
 */

#ifndef LOG_H_
#define LOG_H_


#include "log_internal.h"

#define LOG_INIT(timestamp_func)        LOG_INIT_INTERNAL(timestamp_func)
#define LOG_FLUSH()                     LOG_FLUSH_INTERNAL()

#define LOG_ERROR(...)                  LOG_ERROR_INTERNAL(__VA_ARGS__)
#define LOG_WARNING(...)                LOG_WARNING_INTERNAL(__VA_ARGS__)
#define LOG_INFO(...)                   LOG_INFO_INTERNAL(__VA_ARGS__)
#define LOG_DEBUG(...)                  LOG_DEBUG_INTERNAL(__VA_ARGS__)

#define LOG_ERROR_CHECK(s)              LOG_IF_ERROR(WINSENS_OK != (s), #s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__)
#define LOG_WARNING_CHECK(s)            LOG_IF_WARNING(WINSENS_OK != (s), #s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__)
#define LOG_INFO_CHECK(s)               LOG_IF_INFO(WINSENS_OK != (s), #s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__)
#define LOG_DEBUG_CHECK(s)              LOG_IF_DEBUG(WINSENS_OK != (s), #s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__)

#define LOG_ERROR_RETURN(s, r)          do{ if (WINSENS_OK != s) { LOG_ERROR_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)
#define LOG_WARNING_RETURN(s, r)        do{ if (WINSENS_OK != s) { LOG_WARNING_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)
#define LOG_INFO_RETURN(s, r)           do{ if (WINSENS_OK != s) { LOG_INFO_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)
#define LOG_DEBUG_RETURN(s, r)          do{ if (WINSENS_OK != s) { LOG_DEBUG_INTERNAL(#s "(%u) != WINSENS_OK | %s:%u", s, __FUNCTION__, __LINE__); return r; } }while(0)

#define LOG_IF_ERROR(s, ...)            do{ if (s) { LOG_ERROR_INTERNAL(__VA_ARGS__) } }while(0)
#define LOG_IF_WARNING(s, ...)          do{ if (s) { LOG_WARNING_INTERNAL(__VA_ARGS__) } }while(0)
#define LOG_IF_INFO(s, ...)             do{ if (s) { LOG_INFO_INTERNAL(__VA_ARGS__) } }while(0)
#define LOG_IF_DEBUG(s, ...)            do{ if (s) { LOG_DEBUG_INTERNAL(__VA_ARGS__) } }while(0)

#endif /* LOG_H_ */
