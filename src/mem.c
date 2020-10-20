/*
 * QMem.c
 *
 *  Created on: 2019年2月21日
 *      Author: WanQing
 */
#include <stl/map.h>
#include "qlibc/control.h"
#include "qlibc/mem.h"
#include "std/string.h"
#include "mem_pool.c"
#ifdef MEM_DEBUG
#include "qio.h"
#include "bytelist.h"
qmap *mrecord;

#define insertMsg(n,str,nsize) \
			if (mrecord != NULL) { \
				meminfo* info; \
				sy_malloc(S, &info, sizeof(meminfo));\
				info->msg = str;\
				info->size = nsize;\
				qentry entry;\
				CTRL.stopif(Map.gset(S, mrecord, n, true,&entry));\
				entry._2->val=info;\
			}
#else
#define insertMsg(n,str,nsize) (void)0
#endif
qobj nilobj = { 0 };
void *skym_alloc(State *S, void *ptr, size_t osize, size_t nsize) {
	void* n = NULL;
	sky_assert((osize==0)==(ptr==NULL));
	if (nsize == 0) {
		free(ptr);
	} else {
		if (osize) {
			n = realloc(ptr, nsize);
			if (nsize > osize)
				memset((char*) n + osize, 0, nsize - osize);
		} else {
			n = calloc(1, nsize);
		}
		if (n == NULL)
			skyc_throw(S, ERR_MEM,
					"Unable to allocate memory,If necessary, free memory manually!");
	}
	S->g->gc.GCdebt += nsize - osize;
	return n;
}

static void *alloc_pool(State *S, void *ptr, size_t osize, size_t nsize
#ifdef MEM_DEBUG
		, char *msg
#endif
		) {
	void* n = NULL;
	skyc_assert(S, (osize==0)==(ptr==NULL));
#ifdef MEM_DEBUG
	if (mrecord != NULL && ptr) {
		qentry2 entry;
		if (Map.del(S, mrecord, ptr, &entry)) {
			meminfo *info = cast(meminfo*, entry.val);
			if (info) {
				skyc_assert(S, info->size == osize);
				if (skym_free(S, info) == false) {
					qfree(info);
				}
			}
		} else {
			n = 0; //do nothing
		}
	}
#endif
	if (nsize == 0) {
		if (pool_free(S, ptr) == false) {
			qfree(ptr);
			S->g->gc.GCdebt -= osize;
		}
		return NULL;
	} else { //nsize != 0
		if (osize) {
			if (osize > SMALL_REQUEST_THRESHOLD) {
				if (nsize > SMALL_REQUEST_THRESHOLD) {
					n = realloc(ptr, nsize);
					S->g->gc.GCdebt += nsize - osize;
				} else {
                    pool_malloc(S, &n, nsize);
					memcpy(n, ptr, osize);
					S->g->gc.GCdebt -= osize;
					qfree(ptr);
				}
			} else { //osize <= SMALL_REQUEST_THRESHOLD
				if (nsize > SMALL_REQUEST_THRESHOLD) {
					n = malloc(nsize);
					memcpy(n, ptr, osize);
					S->g->gc.GCdebt += nsize;
					if (pool_free(S, ptr) == false) {
						S->g->gc.GCdebt -= osize;
						free(ptr);
					}
				} else { //nsize <= SMALL_REQUEST_THRESHOLD
					poolp pool = POOL_ADDR(ptr);
					bool isInPool = address_in_range(ptr, pool);
					if (isInPool) {
                        pool_realloc(S, &ptr, nsize);
						insertMsg(ptr, msg, nsize);
						return ptr;
					} else {
                        pool_malloc(S, &n, nsize);
						memcpy(n, ptr, osize);
						S->g->gc.GCdebt -= osize;
						qfree(ptr);
					}
				}
			}
		} else { //osize=0
			if (nsize > SMALL_REQUEST_THRESHOLD) {
				n = malloc(nsize);
				S->g->gc.GCdebt += nsize;
			} else {
                pool_malloc(S, &n, nsize);
			}
		}
		insertMsg(n, msg, nsize);
		if (n == NULL)
			skyc_throw(S, ERR_MEM,
					"Unable to allocate memory,If necessary, free memory manually!");
		if (nsize > osize)
			memset((char*) n + osize, 0, nsize - osize);
		return n;
	}
}

static void *skym_growArray(State *S, void *block, int *size, int n,
		size_t size_elems) {
	void *newblock;
	int oldsize = *size;
	int newsize = (oldsize + n) * 1.7;
	if (newsize < MINSIZEARRAY) {
		newsize = MINSIZEARRAY;
	}
	if (oldsize >= newsize) { /* cannot double it? */
		skyc_throw(S, ERR_RUN, "the size of array overflow");
	}
	newblock = skym_alloc(S, block, oldsize * size_elems, newsize * size_elems);
	*size = newsize; /* update only when everything else is OK */
	return newblock;
}
void *skym_new(State *s, qtype tt, size_t sz) {
	GCObj *o = cast(GCObj*, skym_alloc_pool(s, NULL, 0, sz+sizeof(GCObj)));
	o->marked = currentwhite(s->g);
	o->tt = tt;
	o->data = s->g->gc.allgc;
	s->g->gc.allgc = o;
	return o + 1;
}
/*void skym_destroy(State *s, void *p, int size) {
 skym_alloc_pool(s, o2gc(p), size + sizeof(GCObj), 0);
 }*/

static void skym_gcpro(State *S, GCObj *o) {
	gl_state *g = S->g;
	skyc_assert(S, g->gc.allgc == o);
	gc2gray(o);
	g->gc.allgc = o->data;
	o->data = g->gc.protect;
	g->gc.protect = o;
}

#ifdef MEM_DEBUG
void printMeminfo() {
	mapIter iter;
	Map.iterator(mrecord, &iter);
	printf("total allocated size %ld\n", _S->g->gc.GCdebt);
	while (Map.next(&iter)) {
		qentry2 *entry = iter.entry._2;
		uintptr_t ptr = entry->key;
		if (entry->val) {
			meminfo *info = cast(meminfo *, entry->val);
			printf("adrress %llx size %d msg %s\n", ptr, info->size, info->msg);
		} else {
			printf("delete obsolete adrress %x \n", ptr);
			skyc_assert_(Map.del(_S,mrecord,ptr,NULL));
		}
	}
	if (mrecord->length) {
		qbytes *bytes = Bytes.create(1);
		qentry2 res;
		Map.del(_S, mrecord, bytes, &res);
		skyc_assert_(skym_free(_S, res.val));
		Map.del(_S, mrecord, bytes->data, &res);
		skyc_assert_(skym_free(_S, res.val));
		typeMap->serialize(bytes, mrecord);
		FILE *fp = fopen("res/memInfo.dat", "w");
		fwrite(bytes->data, bytes->length, 1, fp);
		fclose(fp);
		Bytes.destroy(&bytes);
	}
}
void loadMeminfo() {
	FILE *fp = fopen("res/memInfo.dat", "r");
	if (fp) {
		fclose(fp);
		char *bytes = file_read("res/memInfo.dat");
		typeMap->deserial(bytes, &mrecord);
	} else {
		mrecord = Map.create(_S, typeInt, true);
	}
}
const struct apiMem Mem = { skym_new, alloc_pool, printMeminfo, loadMeminfo,
		skym_alloc, skym_gcpro, skym_growArray };
#else
const struct apiMem Mem = { skym_new, alloc_pool, skym_alloc, skym_gcpro,
		skym_growArray };
#endif

