/*
 * bash.h
 *
 *  Created on: Sep 11, 2018
 *      Author: WanQing
 */

#ifndef INCLUDE_BASE_H_
#define INCLUDE_BASE_H_
#ifdef __cplusplus
extern "C"{
#endif
#ifdef __GNUG__
#define NULL __null
#else   /* G++ */
#ifndef __cplusplus
#define NULL ((void *)0)
#else   /* C++ */
#define NULL 0
#endif  /* C++ */
#endif  /* G++ */
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include<limits.h>
#include<stdlib.h>
#include<stdint.h>

#define  STRINGIFY(x) #x
#define  TOSTRING(x) STRINGIFY(x)
#define  AT __FILE__ ":" TOSTRING(__LINE__)
//#define  AT __FILE__ ":" #__FUNCTION__" "TOSTRING(__LINE__)
#define sky_error(str,code) \
	do{fprintf(stdout,"%s %s\ttype:%d\n",AT,str,code);exit(code);}while(0)

#define sky_assert(expr)							\
  ({			\
      if (!(expr))								\
       {\
    	 printf("%s:"#expr"\n",AT);\
    	 exit(1);\
      }								\
    })

#define sky_check(expr,str)							\
  ({			\
      if (!(expr))								\
       {\
    	 fprintf(stdout, "%s "#expr" %s\n",AT,str);\
    	 exit(1);\
      }								\
    })

#define qfree free
#define qmalloc(t) (t*)malloc(sizeof(t))
#define qmalloc2(t,n) (t*)malloc(n*sizeof(t))
#define qcalloc(t) (t*)calloc(1,sizeof(t))
#define qcalloc2(t,n) (t*)calloc(n,sizeof(t))
#define cast(t,exp) ((t)(exp))

typedef unsigned int uint;
typedef unsigned char byte;

typedef enum {
false,true}bool;
typedef ssize_t Int;
typedef size_t UInt;
#define RInt register Int
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_BASE_H_ */
