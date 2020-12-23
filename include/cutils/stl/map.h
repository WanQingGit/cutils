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


#define gnode(t,i)	((t)->table+i)
/* 'const' to avoid wrong writings that can mess up field 'next' */
#define gkey(n)		(&(n)->key)
#define gentry(t,h) (t->entry[(t->size-1)&h])
#define gentry1(t,h) gentry(t,h)._1
#define gentry2(t,h) gentry(t,h).dict
//#define gentry(t,h) (t->table+(t->size-1)&h)
#define gnext(n)	((n)->next)
#define gval(n)		(&(n)->val)
#define map_set_val(entry,v) entry.dict->val=(qobj*)(v);
#define map_set_key(entry,v) entry.dict->key=(qobj*)(v);
#define map_get_val(entry,v) v=(typeof(v))((entry).dict->val)
#define map_get_key(entry,v) v=(typeof(v))((entry).dict->key)
#define map_gsetInt(S,t,key,v) ({ \
	qobj _o=OBJ(key,typeInt);skyt_gset(S,t,&_o,v); \
})
#define map_gsetStr(S,t,key,v) ({ \
	qobj _o=OBJ(key,typeString);skyt_gset(S,t,&_o,v); \
})
#define map_keytype(t) *cast(Type*,cast(qmap *,t)+1)


/**
* qentry_dict,4用于节省内存，使用keytype保存类型
* qentry1,2,3,4的next和key的位置一定要一致
* 不一致遍历会出错
* 1,2用于map,3,4用于set
* qentry的key除非能保证hash和compare和原key相同
* 不然一定不要更改，切记！
*/
/**
 * deprecated
 * 没有必要存储类型信息
 */
//typedef struct qentry1 {
//    struct qentry1 *next;
//    qobj key;
//    qobj val;
//} qentry1;

//typedef struct qentry3 {
//    struct qentry3 *next;
//    qobj key;
//} qentry3;

typedef struct qentry_dict {
    struct qentry_dict *next;
    qobj *key;
    qobj *val;
} qentry_dict;


typedef struct qentry_set {
    struct qentry_set *next;
    qobj *key;
} qentry_set;
typedef union qentry {
//    qentry1 *_1;
    qentry_set *set;
    qentry_dict *dict;
} qentry;



/**
* 第一位用于判断是否是table
* 第二位用于判断是否使用keytype
* map创建后不要修改该值
*/
typedef enum {
    MAP_SET = 0, MAP_TABLE = 1, MAP_OBJTYPE = 2,MAP_FREE_VAL=4,MAP_FREE_KEY=8,MAP_FREE_FORCE=16
} MapType;
/**
 * 当存放的对象不是qobj对象时，使用keytype来实现增删改查
 * map一旦创建，keytype不能修改，valtype可修改,所以keytype不可见
 * keytype不为null,则插入的key必须满足该类型，插入的值是原生不带类型的。
 * keytype为null,则值必须带类型。
 * valtype用于序列化，仅当keytype存在时有效
 */
struct QHashMap {
    qentry *entry;
    Type valtype;
    uint size;
    uint length;
    MapType type;
};
//#define map_keytype(t) ((t)->keytype)
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
struct MapInterface {
	qmap* (*create)(Type keytype, MapType mode);
	void (*resize)(qmap *t, size_t size);
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
	bool (*gset)(qmap *t, const void *key, bool insert, qentry *res);
//	qentry (*gset)(State *S, qmap *t, const qobj *key, bool isert);
	bool (*del)(qmap *t, const void *key, qentry_dict *res);
//	bool (*del)(State *S, qmap *t, const qobj *key, qobj *keyval);
	void (*iterator)(qmap *t, mapIter *iter);
	bool (*next)(mapIter *iter);
	void (*destroy)(qmap *map);
};

extern struct MapInterface Map;
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QMAP_H_ */
