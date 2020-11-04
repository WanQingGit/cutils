/*
 * qtable.c
 *
 *  Created on: 2019年3月12日
 *      Author: WanQing
 */
#include <stl/map.h>
#include "qlibc/object.h"
#include "utils/math.h"
#include "qlibc/mem.h"
#include "qlibc/control.h"
#define MAP_MINSIZE 4
#define uentry(t) t->entry
static void map_resize(State *S, qmap *t, unsigned int size) {
	int oldsize = t->size, hash;
	Type keytype = map_keytype(t);
	int sizeht = MAP_MINSIZE;
	for (; sizeht < size && sizeht > 0; sizeht <<= 1)
		;
	if (sizeht< 0)
		qrunerror(S, "table overflow");
	if (sizeht != oldsize) {
		qentry *oldt = uentry(t);
		uentry(t) = skym_newvector(S, sizeht, qentry);
		t->size = sizeht;
		if (t->length) {
			int mod = sizeht - 1;
			if (!keytype) {
				qentry1 *node, *temp;
				for (int i = 0; i < oldsize; i++) {
					node = oldt[i]._1;
					while (node) {
						hash = node->key.type->hash(node->key.val.i) & mod;
						temp = node->next;
						node->next = uentry(t)[hash]._1;
						uentry(t)[hash]._1 = node;
						node = temp;
					}
				}
			} else {
				qentry2 *node, *temp;
				for (int i = 0; i < oldsize; i++) {
					node = oldt[i]._2;
					while (node) {
						hash = keytype->hash(node->key) & mod;
						temp = node->next;
						node->next = uentry(t)[hash]._2;
						uentry(t)[hash]._2 = node;
						node = temp;
					}
				}
			}
		}
		if (oldsize)
            Mem.alloc(S, oldt, oldsize * sizeof(qentry), 0);
	}
}

static bool map_del(State *S, qmap *t, const qobj *key, qentry2 *res) {
	Type keytype = map_keytype(t);
	qobj v = { 0 };
	if (!keytype) {
		uint hash = key->type->hash(key->val.i); //sky_hash(key);
		qentry1 *entry = gentry(t, hash)._1, *prev = NULL;
		while (entry) {
			if (entry->key.type == key->type
					&& entry->key.type->compare(entry->key.val.i, key->val.i) == 0) {
				if (prev)
					prev->next = entry->next;
				else
					gentry2(t, hash) = entry->next;
				if (res) {
					res->key = &entry->key;
					if (t->type & MAP_TABLE)
						res->val = &entry->val;
				}
                Mem.alloc(S, entry, sizeof(qentry1), 0);
//				free(entry);
				t->length--;
				return true;
			}
			prev = entry;
			entry = entry->next;
		}
	} else {
		uint hash = keytype->hash(key); //sky_hash(key);
		qentry2 *entry = gentry2(t, hash);
		qentry2 *prev = NULL;
		while (entry) {
			if (keytype->compare(entry->key, key) == 0) {
				if (prev)
					prev->next = entry->next;
				else
					gentry(t, hash)._2 = entry->next;
				if (res) { //keyval指向的变量必须要大于sizeof(void*)
					res->key = entry->key;
					if (t->type & MAP_TABLE) {
						res->val = entry->val;
					}
				}
                Mem.alloc(S, entry, sizeof(qentry2), 0);
				t->length--;
				return true;
			}
			prev = entry;
			entry = entry->next;
		}
	}
	return false;
}

static bool map_gset(State *S, qmap *t, const qobj *key, bool insert,
		qentry *res) {
	if (t->length == 0 && insert == false)
		return false;
	Type keytype = map_keytype(t);
	if (keytype) {
		uint hash = keytype->hash(key);
		qentry2 *entry2 = gentry2(t, hash);
		while (entry2) {
			if ((keytype->compare(entry2->key, key) == 0)) {
				res->_2 = entry2;
				return true;
			}
			entry2 = entry2->next;
		}
		if (insert) {
			if (t->length >= t->size) {
				map_resize(S, t, t->size * 2);
			}
			if (t->type & MAP_TABLE) {
				entry2 = (qentry2*) Mem.alloc(S, NULL, 0, sizeof(qentry2));
			} else {
				entry2 = (qentry2*) Mem.alloc(S, NULL, 0, sizeof(qentry4));
			}
			entry2->key = key;
			entry2->next = gentry2(t, hash);
			++t->length;
			gentry2(t, hash) = entry2;
			res->_2 = entry2;
			return false;
		}
		//t->keytype
	} else {
		qassert_(S, key != NULL && key->type->compare&&key->type->hash);
		uint hash = key->type->hash(key->val.i);
		qentry1 *entry = gentry1(t, hash);
		while (entry) {
			if (entry->key.type == key->type) {
				if (entry->key.val.gc == key->val.gc
						|| (key->type->compare(entry->key.val.i, key->val.i) == 0)) {
					res->_1 = entry;
					return true;
				}
			}
			entry = entry->next;
		}
		if (insert) {
			if (t->length >= t->size) {
				map_resize(S, t, t->size * 2);
			}
			++t->length;
			if (t->type & MAP_TABLE) {
				entry = (qentry1*) Mem.alloc(S, NULL, 0, sizeof(qentry1));
			} else {
				entry = (qentry1*) Mem.alloc(S, NULL, 0, sizeof(qentry3));
			}
			entry->next = gentry1(t, hash);
			gentry1(t, hash) = entry;
			entry->key = *key;
			res->_1 = entry;
			return false;
		}
	}
	return false;
}
static void map_iter(qmap *t, mapIter *iter) {
	iter->m = t;
	iter->keytype = map_keytype(t);
	iter->index = 0;
	iter->entry._1 = iter->nextEntry._1 = NULL;
}
static bool map_next(mapIter *iter) {
	if (iter->index < iter->m->size) {
		if (iter->nextEntry._1) {
			iter->entry = iter->nextEntry;
			iter->nextEntry._1 = iter->nextEntry._1->next;
			return true;
		} else {
			int i;
			for (i = iter->index; i < iter->m->size; i++) {
				if (iter->m->entry[i]._1) {
					iter->entry = iter->m->entry[i];
					iter->nextEntry._1 = iter->entry._1->next;
					iter->index = i + 1;
					return true;
				}
			}
			iter->index = i;
		}
	}
	return false;
}
static qmap* map_new(State *S, Type keytype, bool isTable) {
	qmap *t = cast(qmap*, Mem.alloc(S, NULL, 0, sizeof(qmap) + sizeof(Type)));
//	qmap *t = cast(qmap*, Mem.new(S, V_TABLE, sizeof(qmap) + sizeof(Type)));
	map_keytype(t) = keytype;
	if (isTable)
		t->type |= MAP_TABLE;
	if (keytype == NULL)
		t->type |= MAP_OBJTYPE;
	map_resize(S, t, MINTABLESIZE);
	return t;
}
static void map_destroy(qmap *map, int freeMode) {
	if (map->length) {
		Type keytype = map_keytype(map);
		int entrysize;
		if (keytype) {
			if (map->type & MAP_TABLE)
				entrysize = sizeof(qentry2);
			else {
				entrysize = sizeof(qentry4);
			}
			qentry2 *entry2, *ptr;
			Type valtype;
			if (freeMode & MAP_FREE_VAL)
				qassert(valtype = map->valtype);

			for (int i = 0; i < map->size; i++) {
				entry2 = map->entry[i]._2;
				while (entry2) {
					ptr = entry2;
					entry2 = entry2->next;
					if (freeMode & MAP_FREE_KEY) {
						keytype->free(ptr->key);
					}
					if (freeMode & MAP_FREE_VAL) {
						valtype->free(ptr->val);
					}
                    Mem.alloc(_S, ptr, entrysize, 0);
				}
			}
		} else {
			if (map->type & MAP_TABLE)
				entrysize = sizeof(qentry1);
			else
				entrysize = sizeof(qentry3);
			qentry1 *entry1, *ptr;
			for (int i = 0; i < map->size; i++) {
				entry1 = map->entry[i]._1;
				while (entry1) {
					ptr = entry1;
					entry1 = entry1->next;
					if (freeMode & MAP_FREE_KEY) {
						ptr->key.type->free(ptr->key.val.i);
					}
					if (freeMode & MAP_FREE_VAL) {
						ptr->val.type->free(ptr->val.val.i);
					}
                    Mem.alloc(_S, ptr, entrysize, 0);
				}
			}
		}
	}
    Mem.alloc(_S, map->entry, map->size * sizeof(qentry), 0);
	Mem.alloc(_S, map, sizeof(qmap) + sizeof(Type), 0);
}
struct apiMap Map = { map_new, map_resize, map_gset, map_del, map_iter,
		map_next, map_destroy };
