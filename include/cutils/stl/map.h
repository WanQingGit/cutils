/*
 * qmap.h
 *
 *  Created on: 2019年3月13日
 *      Author: WanQing
 */

#ifndef INCLUDE_QMAP_H_
#define INCLUDE_QMAP_H_
#ifdef __cplusplus
extern "C"{
#endif
#include "cutils/object.h"

#define MAP_NOT_FREE 0
#define MAP_FREE_KEY 1
#define MAP_FREE_VAL 1<<1
#define MAP_FREE_FORCE 1<<2
#define MAP_FREE_BOTH MAP_FREE_KEY|MAP_FREE_VAL
#define MAP_FREE_MAX  MAP_FREE_BOTH|MAP_FREE_FORCE

#define gnode(t,i)	((t)->table+i)
/* 'const' to avoid wrong writings that can mess up field 'next' */
#define gkey(n)		(&(n)->key)
#define gentry(t,h) (t->entry[(t->size-1)&h])
#define gentry1(t,h) gentry(t,h)._1
#define gentry2(t,h) gentry(t,h)._2
//#define gentry(t,h) (t->table+(t->size-1)&h)
#define gnext(n)	((n)->next)
#define gval(n)		(&(n)->val)
#define map_gsetInt(S,t,key,v) ({ \
	qobj _o=OBJ(key,typeInt);skyt_gset(S,t,&_o,v); \
})
#define map_gsetStr(S,t,key,v) ({ \
	qobj _o=OBJ(key,typeString);skyt_gset(S,t,&_o,v); \
})
#define map_keytype(t) *cast(Type*,cast(qmap *,t)+1)
typedef struct mapIter {
	qmap *m;
	Type keytype;
	int index;
	qentry entry;
	qentry nextEntry;
} mapIter;
//typedef enum {
//	MAP_UPDATE, MAP_GET, MAP_GSET
//} MapMode;
struct apiMap {
	qmap* (*create)(State *S, Type keytype, bool isTable);
	void (*resize)(State *S, qmap *t, uint size);
	/**
	 * 获取map中key的值，
	 * 不存在根据参数mode是否插入
	 * 如果要替换entry的key,一定要保证hash,compare相等
	 * 如果不使用gset,则需要判断是否存在，
	 * 插入的时候又要比较键值判断是否存在再插入
	 * 需要调用两次函数，比较两次，效率比较低
	 * 返回值：
	 * 		如果键值存在，返回true,不存在返回false
	 */
	bool (*gset)(State *S, qmap *t, const qobj *key, bool insert, qentry *res);
//	qentry (*gset)(State *S, qmap *t, const qobj *key, bool isert);
	bool (*del)(State *S, qmap *t, const qobj *key, qentry2 *res);
//	bool (*del)(State *S, qmap *t, const qobj *key, qobj *keyval);
	void (*iterator)(qmap *t, mapIter *iter);
	bool (*next)(mapIter *iter);
	void (*destroy)(qmap *map, int freeMode);
};

extern struct apiMap Map;
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QMAP_H_ */
