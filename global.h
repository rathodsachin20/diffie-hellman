#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <openssl/dh.h>
#include <openssl/bn.h>
#include <time.h>

typedef struct timespec timespec;

/* IETF standardized parameters for D-H key exchange [RFC5114]
*  Link: http://tools.ietf.org/html/rfc5114#section-2.1
*/

extern const char df_p_hex1024_str[];

// 160-bit prime order
extern const char df_g_hex1024_str[];

extern const char df_p_hex2048_str[];

// 224-bit prime order
extern const char df_g_hex2048_str[];

timespec diff(timespec start, timespec end);
timespec add(timespec t1, timespec t2);

#endif
