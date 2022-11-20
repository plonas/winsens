/*
 * utils.h
 *
 *  Created on: 18.10.2018
 *      Author: Damian Plonek
 */

#ifndef UTILS_H_
#define UTILS_H_


#define UTILS_ASSERT(x)         while(false == (x))

#define UTILS_STRING(x)         #x
#define UTILS_CONCAT2(a, b)     a ## b

#define UTILS_UNUSED_VAR(x)     ((void)x)

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#endif


#endif /* UTILS_H_ */
