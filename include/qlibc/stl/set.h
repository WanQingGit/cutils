/*
 * qhashset.h
 *
 *  Created on: May 17, 2019
 *  Author: WanQing<1109162935@qq.com>
 */

#ifndef INCLUDE_QHASHSET_H_
#define INCLUDE_QHASHSET_H_
#ifdef __cplusplus
extern "C"{
#endif
#include "qlibc/object.h"

typedef struct qsetEntry{
	qobj data;
	struct qsetEntry *next;
}qsetEntry;
typedef struct qhashset{
	int capacity;
	int length;
	qsetEntry **entry;
}qset;

struct funcSet{
void (*resize)(State *S, qset *t, unsigned int size);
bool (*del)(State *S, qset *t, const qobj *key);
bool (*contain)(State *S, qset *t, const qobj *key, bool isAdd);
};
extern struct funcSet SET;
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QHASHSET_H_ */
