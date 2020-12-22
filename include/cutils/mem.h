/*
 *
 *  Created on: 2019年2月21日
 *      Author: WanQing
 */
#ifndef INCLUDE_QMEM_H_
#define INCLUDE_QMEM_H_
#ifdef __cplusplus
extern "C"{
#endif

#include "cutils/object.h"

#define MINSIZEARRAY    4
#define skym_newvector( n, t) \
        cast(t *, Mem.alloc(NULL, 0,n*sizeof(t)))
#define mem_realloc_vector( v, oldn, n, t) \
   ((v)=cast(t *, Mem.alloc(v,oldn*sizeof(t),n*sizeof(t))))

//void *mem_new(State *s, qtype tt, size_t sz);
//void *Mem.alloc(State *S, void *ptr, size_t osize, size_t nsize);

struct apiMem {
    void (*enable_pool)(bool enable);

    void *(*new)( Type tt, size_t sz);

    /*
     * 将链头o对象放入到链上
     * 如果不是GCObj,一定要在之前调用o2gc(o)进行转换
     */
    void *(*alloc)( void *ptr, size_t osize, size_t nsize);
    /**
     * 类似于qbyte,但只能增长，需要使用变量单独保存size，必要时保存已使用的个数
     * qbyte则无需考虑，但qbyte效率要比growArray低些，growArray适用于只增不减的数组
     */
    void *(*growArray)(void *block, int *size, int n,
                       size_t size_elems);
};

extern const struct apiMem Mem;

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QMEM_H_ */
