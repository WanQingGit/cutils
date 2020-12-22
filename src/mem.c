/*
 *  Created on: 2019年2月21日
 *      Author: WanQing
 */
#include <stl/map.h>
#include "cutils/control.h"
#include "cutils/mem.h"
#include "mem_pool.c"
#include <string.h>

qobj nilobj = {0};
static bool pool_enable = 0;

void *normal_alloc(void *ptr, size_t osize, size_t nsize) {
    void *n = NULL;
    qassert((osize == 0) == (ptr == NULL));
    if (nsize == 0) {
        free(ptr);
    } else {
        if (osize) {
            n = realloc(ptr, nsize);
            if (nsize > osize)
                memset((char *) n + osize, 0, nsize - osize);
        } else {
            n = calloc(1, nsize);
        }
        if (n == NULL)
            qthrow(_S, ERR_MEM,
                   "Unable to allocate memory,If necessary, free memory manually!");
    }
    _G->gc.GCdebt += nsize - osize;
    return n;
}

static void *mem_alloc( void *ptr, size_t osize, size_t nsize) {
    if (!pool_enable)
        return normal_alloc( ptr, osize, nsize);
    void *n = NULL;
    qassert_(_S, (osize == 0) == (ptr == NULL));
    if (nsize == 0) {
        if (pool_free(ptr) == false) {
            qfree(ptr);
            _G->gc.GCdebt -= osize;
        }
        return NULL;
    } else { //nsize != 0
        if (osize) {
            if (osize > SMALL_REQUEST_THRESHOLD) {
                if (nsize > SMALL_REQUEST_THRESHOLD) {
                    n = realloc(ptr, nsize);
                    _G->gc.GCdebt += nsize - osize;
                } else {
                    pool_malloc(&n, nsize);
                    memcpy(n, ptr, osize);
                    _G->gc.GCdebt -= osize;
                    qfree(ptr);
                }
            } else { //osize <= SMALL_REQUEST_THRESHOLD
                if (nsize > SMALL_REQUEST_THRESHOLD) {
                    n = malloc(nsize);
                    memcpy(n, ptr, osize);
                    _G->gc.GCdebt += nsize;
                    if (pool_free(ptr) == false) {
                        _G->gc.GCdebt -= osize;
                        free(ptr);
                    }
                } else { //nsize <= SMALL_REQUEST_THRESHOLD
                    poolp pool = POOL_ADDR(ptr);
                    bool isInPool = address_in_range(ptr, pool);
                    if (isInPool) {
                        pool_realloc(&ptr, nsize);
                        return ptr;
                    } else {
                        pool_malloc(&n, nsize);
                        memcpy(n, ptr, osize);
                        _G->gc.GCdebt -= osize;
                        qfree(ptr);
                    }
                }
            }
        } else { //osize=0
            if (nsize > SMALL_REQUEST_THRESHOLD) {
                n = malloc(nsize);
                _G->gc.GCdebt += nsize;
            } else {
                pool_malloc(&n, nsize);
            }
        }
        if (n == NULL)
            qthrow(_S, ERR_MEM,
                   "Unable to allocate memory,If necessary, free memory manually!");
        if (nsize > osize)
            memset((char *) n + osize, 0, nsize - osize);
        return n;
    }
}

static void *mem_growArray(void *block, int *size, int n,
                           size_t size_elems) {
    void *newblock;
    int oldsize = *size;
    int newsize = (oldsize + n) * 1.7;
    if (newsize < MINSIZEARRAY) {
        newsize = MINSIZEARRAY;
    }
    if (oldsize >= newsize) { /* cannot double it? */
        qthrow(_S, ERR_RUN, "the size of array overflow");
    }
    newblock = Mem.alloc(block, oldsize * size_elems, newsize * size_elems);
    *size = newsize; /* update only when everything else is OK */
    return newblock;
}

void *mem_new( Type tt, size_t sz) {
    GCNode *o = cast(GCNode*, Mem.alloc( NULL, 0, sz + sizeof(GCNode)));
    o->size=sz;
    o->type = tt;
    o->next = _G->gc.allgc;
    _G->gc.allgc = o;
    return o + 1;
}

void mem_enable_pool(bool enable) {
    pool_enable = enable;
}

const struct apiMem Mem = {mem_enable_pool, mem_new, mem_alloc,
                           mem_growArray};
