/*
 *
 *  Created on: 2019年3月14日
 *      Author: WanQing
 */


/*
** Size of cache for strings in the API. 'N' is the number of
** sets (better be a prime) and "M" is the size of each set (M == 1
** makes a direct cache.)
*/
#if !defined(STRCACHE_N)
#define STRCACHE_N		64
#define STRCACHE_M		2
#endif
#ifndef MAXSHORTLEN
#define MAXSHORTLEN	40
#endif
#ifndef MINTABLESIZE
#define MINTABLESIZE	4
#endif
#ifndef INT
#define INT	long long
#endif
#ifndef FLT
#define FLT	double
#endif

