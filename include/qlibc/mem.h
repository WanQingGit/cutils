/*
 *
 *  Created on: 2019年2月21日
 *      Author: WanQing
 */
//#define MEM_DEBUG

#ifndef INCLUDE_QMEM_H_
#define INCLUDE_QMEM_H_
#ifdef __cplusplus
extern "C"{
#endif

#include "qlibc/object.h"

#define MINSIZEARRAY    4
#define bitmask(b)        (1<<(b))
#define WHITE0BIT    0  /* object is white (type 0) */
#define WHITE1BIT    1  /* object is white (type 1) */
#define BLACKBIT    2  /* object is black */
#define bit2mask(b1, b2)        (bitmask(b1) | bitmask(b2))
#define WHITEBITS    bit2mask(WHITE0BIT, WHITE1BIT)
#define currentwhite(s)     ((s)->gc.currentwhite& WHITEBITS)
#define otherwhite(s)     ((3-(s)->gc.currentwhite)& WHITEBITS)
#define gc2gray(o) (o->marked=4)
#define isdead(g, v)    (otherwhite(g)&(v)->marked)
#define changewhite(o) (o)->marked=WHITEBITS& (WHITEBITS^((o)->marked))
#define skym_newvector(S, n, t) \
        cast(t *, Mem.alloc(S,NULL, 0,n*sizeof(t)))

/*
 #define skym_growvector(L,v,nelems,size,t,limit) \
          if ((nelems)+1 > (size)) \
            ((v)=(t *)(skym_growarr(L,v,&(size),sizeof(t),limit,NULL)))
 */


//#define Mem.alloc(S, ptr, osize, nsize) Mem.alloc(S,ptr,osize,nsize)
#define qalloc(size)  Mem.alloc(_S,NULL,0,size)
#define mem_calloc(S, t, n) \
    cast(t*,Mem.alloc(S,NULL,0,n*sizeof(t)))
#define mem_realloc_vector(L, v, oldn, n, t) \
   ((v)=cast(t *, Mem.alloc(L,v,oldn*sizeof(t),n*sizeof(t))))

//void *mem_new(State *s, qtype tt, size_t sz);
//void *Mem.alloc(State *S, void *ptr, size_t osize, size_t nsize);

struct apiMem {
    void (*enable_pool)(bool enable);

    void *(*new)(State *s, qtype tt, size_t sz);


    void *(*alloc)(State *S, void *ptr, size_t osize, size_t nsize);

    /*
     * 将链头o对象放入到保护链上,对象将
     * 不会被回收
     * 如果不是GCObj,一定要在之前调用o2gc(o)进行转换
     */
    void (*protect)(State *S, GCObj *o);

    /**
     * 类似于qbyte,但只能增长，需要使用变量单独保存size，必要时保存已使用的个数
     * qbyte则无需考虑，但qbyte效率要比growArray低些，growArray适用于只增不减的数组
     */
    void *(*growArray)(State *S, void *block, int *size, int n,
                       size_t size_elems);
};

extern const struct apiMem Mem;

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QMEM_H_ */
