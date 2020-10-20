/*
 * Qmath.h
 *
 *  Created on: Sep 12, 2018
 *      Author: WanQing
 */

#ifndef INCLUDE_QMATH_H_
#define INCLUDE_QMATH_H_
#ifdef __cplusplus
extern "C"{
#endif
#include "qlibc/object.h"
#define Q_MAX(A,B) ({\
	typeof(A) A_=A;\
	typeof(B) B_=B;\
	A_>B_?A_:B_;\
})
#define Q_MIN(A,B) ({\
	typeof(A) A_=A;\
	typeof(B) B_=B;\
	A_<B_?A_:B_;\
})
#define Q_POW(x,y) ({\
	typeof(x) n=1;\
	for(int i=0;i<y;i++){\
		n*=x;}\
	n;})

#define MASKN(n,p)	((~((~(unsigned long long)0)<<(n)))<<(p))
#define MASK(B)		(1 << (B))
#define BIT(i,pos,size)	((i>>pos) & MASKN(size,0))
int ceillog2 (unsigned int x);
uint sky_hash(const qobj *o);

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QMATH_H_ */
