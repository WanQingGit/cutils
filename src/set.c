/*
 * hashSet.c
 *
 *  Created on: Apr 17, 2019
 *      Author: WanQing
 */

#include "qlibc/object.h"
#include "utils/math.h"
#include "stl/set.h"
#include "qlibc/mem.h"
#include "qlibc/control.h"
#define gentry2(t,h) (t->entry[(t->capacity-1)&h])
static void set_resize(State *S, qset *t, unsigned int size) {
	int oldsize = t->capacity, hash;
	qsetEntry **oldt = t->entry, *node, *temp;
	int sizeht = sizearr(ceillog2(size));
	int mod = sizeht - 1;
	if (sizeht > INT_MAX)
		qrunerror(S, "table overflow");
	if (sizeht != oldsize) {
		t->entry = skym_newvector(S, sizeht, qsetEntry*);
		t->capacity = sizeht;
		for (int i = 0; i < oldsize; i++) {
			node = oldt[i];
			while (node) {
				hash = sky_hash(&node->data) & mod;
				temp = node->next;
				node->next = t->entry[hash];
				t->entry[hash] = node;
				node = temp;
			}
		}
		if (oldsize)
            Mem.alloc(S, oldt, oldsize * sizeof(qsetEntry *), 0);
	}
}

static bool set_del(State *S, qset *t, const qobj *key) {
	uint hash = sky_hash(key);
	qsetEntry *val = gentry2(t, hash);
	qsetEntry *prev = NULL;
	while (val) {
		if (val->data.type->baseType == key->type->baseType && val->data.val.gc == key->val.gc) {
			if (prev)
				prev->next = val->next;
			else
				gentry2(t, hash) = NULL;
			free(val);
			return true;
		}
		prev = val;
		val = val->next;
	}
	return false;
}
static bool set_contain(State *S, qset *t, const qobj *key, bool isAdd) {
	if (t->length == 0) {
		set_resize(S, t, 2);
	}
	qassert_(S, key != NULL && key->type != V_NIL);
	uint hash = sky_hash(key);
	qsetEntry *entry = gentry2(t, hash);
	if (key->type->compare) {
		while (entry) {
			if (entry->data.val.gc == key->val.gc
					|| key->type->compare(key, &entry->data)) {
				return true;
			}
			entry = entry->next;
		}
	} else {
		while (entry) {
			if (entry->data.type == key->type && entry->data.val.gc == key->val.gc) {
				return true;
			}
			entry = entry->next;
		}
	}

	if (isAdd) {
		if (t->length >= t->capacity) {
			set_resize(S, t, t->capacity * 2);
		}
		entry = (qsetEntry*) Mem.alloc(S, NULL, 0, sizeof(qsetEntry));
		entry->data = *key;
		entry->next = gentry2(t, hash);
		++t->length;
		gentry2(t, hash) = entry;
	}
	return false;
}
struct funcSet SET={set_resize,set_del,set_contain};
