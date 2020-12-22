/*
 * qtable.c
 *
 *  Created on: 2019年3月12日
 *      Author: WanQing
 */
#include <stl/map.h>
#include "cutils/object.h"
#include "cutils/mem.h"
#include "cutils/control.h"

#define MAP_MINSIZE 4
#define uentry(t) t->entry
#define size_entry(t) ((t)->type&MAP_TABLE)?sizeof(qentry_dict):sizeof(qentry_set)
#define size_map (sizeof(qmap) + sizeof(Type))

static void map_resize(qmap *t, size_t size) {
  int oldsize = t->size, hash;
  Type keytype = map_keytype(t);
  int sizeht = MAP_MINSIZE;
  for (; sizeht < size && sizeht > 0; sizeht <<= 1);
  if (sizeht < 0)
    qrunerror(_S, "table overflow");
  if (sizeht != oldsize) {
    qentry *oldt = uentry(t);
    uentry(t) = skym_newvector(sizeht, qentry);
    t->size = sizeht;
    if (t->length) {
      int mod = sizeht - 1;
      if (!keytype) {
        qentry_dict *node, *temp;
        for (int i = 0; i < oldsize; i++) {
          node = oldt[i].dict;
          while (node) {
            hash = node->key->type->hash(node->key->val.i) & mod;
            temp = node->next;
            node->next = uentry(t)[hash].dict;
            uentry(t)[hash].dict = node;
            node = temp;
          }
        }
      } else {
        qentry_dict *node, *temp;
        for (int i = 0; i < oldsize; i++) {
          node = oldt[i].dict;
          while (node) {
            hash = keytype->hash(node->key) & mod;
            temp = node->next;
            node->next = uentry(t)[hash].dict;
            uentry(t)[hash].dict = node;
            node = temp;
          }
        }
      }
    }
    if (oldsize)
      Mem.alloc(oldt, oldsize * sizeof(qentry), 0);
  }
}

static bool map_del(qmap *t, const qobj *key, qentry_dict *res) {
  Type keytype = map_keytype(t);
  qobj v = {0};
  if (!keytype) {
    size_t hash = key->type->hash(key->val.p);
    qentry_dict *entry = gentry(t, hash).dict, *prev = NULL;
    while (entry) {
      if (entry->key->type == key->type
          && entry->key->type->compare(entry->key->val.i, key->val.i) == 0) {
        if (prev)
          prev->next = entry->next;
        else
          gentry2(t, hash) = entry->next;
        if (res) {
          res->key = &entry->key;
          if (t->type & MAP_TABLE)
            res->val = &entry->val;
        }
        Mem.alloc(entry, size_entry(t), 0);
        t->length--;
        return true;
      }
      prev = entry;
      entry = entry->next;
    }
  } else {
    size_t hash = keytype->hash(key);
    qentry_dict *entry = gentry2(t, hash);
    qentry_dict *prev = NULL;
    while (entry) {
      if ((entry->key == key) || keytype->compare(entry->key, key) == 0) {
        if (prev)
          prev->next = entry->next;
        else
          gentry(t, hash).dict = entry->next;
        if (res) { //keyval指向的变量必须要大于sizeof(void*)
          res->key = entry->key;
          if (t->type & MAP_TABLE) {
            res->val = entry->val;
          }
        }

        Mem.alloc(entry, size_entry(t), 0);
        t->length--;
        return true;
      }
      prev = entry;
      entry = entry->next;
    }
  }
  return false;
}

static bool map_gset(qmap *t, const qobj *key, bool insert,
                     qentry *res) {
  if (t->length == 0 && insert == false)
    return false;
  Type keytype = map_keytype(t);
  if (keytype) {
    size_t hash = keytype->hash(key);
    qentry_dict *entry2 = gentry2(t, hash);
    while (entry2) {
      if ((keytype->compare(entry2->key, key) == 0)) {
        res->dict = entry2;
        return true;
      }
      entry2 = entry2->next;
    }
    if (insert) {
      if (t->length >= t->size) {
        map_resize(t, t->size * 2);
      }
      if (t->type & MAP_TABLE) {
        entry2 = (qentry_dict *) Mem.alloc(NULL, 0, sizeof(qentry_dict));
      } else {
        entry2 = (qentry_dict *) Mem.alloc(NULL, 0, sizeof(qentry_set));
      }
      entry2->key = key;
      entry2->next = gentry2(t, hash);
      ++t->length;
      gentry2(t, hash) = entry2;
      res->dict = entry2;
      return false;
    }
    //t->keytype
  } else {
    qassert_(_S, key != NULL && key->type->compare && key->type->hash);
    size_t hash = key->type->hash(key->val.p);
    qentry_dict *entry = gentry2(t, hash);
    while (entry) {
      if (entry->key->type == key->type) {
        if (entry->key->val.p == key->val.p
            || (key->type->compare(entry->key->val.p, key->val.p) == 0)) {
          res->dict = entry;
          return true;
        }
      }
      entry = entry->next;
    }
    if (insert) {
      if (t->length >= t->size) {
        map_resize(t, t->size * 2);
      }
      ++t->length;
      if (t->type & MAP_TABLE) {
        entry = (qentry_dict *) Mem.alloc(NULL, 0, sizeof(qentry_dict));
      } else {
        entry = (qentry_dict *) Mem.alloc(NULL, 0, sizeof(qentry_set));
      }
      entry->next = gentry2(t, hash);
      gentry2(t, hash) = entry;
      entry->key = key;
      res->dict = entry;
      return false;
    }
  }
  return false;
}

static void map_iter(qmap *t, mapIter *iter) {
  iter->m = t;
  iter->keytype = map_keytype(t);
  iter->index = 0;
  iter->entry.dict = iter->nextEntry.dict = NULL;
}

static bool map_next(mapIter *iter) {
  if (iter->index < iter->m->size) {
    if (iter->nextEntry.dict) {
      iter->entry = iter->nextEntry;
      iter->nextEntry.dict = iter->nextEntry.dict->next;
      return true;
    } else {
      int i;
      for (i = iter->index; i < iter->m->size; i++) {
        if (iter->m->entry[i].dict) {
          iter->entry = iter->m->entry[i];
          iter->nextEntry.dict = iter->entry.dict->next;
          iter->index = i + 1;
          return true;
        }
      }
      iter->index = i;
    }
  }
  return false;
}

static qmap *map_new(Type keytype, MapType type) {
  qmap *t = cast(qmap*, Mem.alloc(NULL, 0, size_map));
  map_keytype(t) = keytype;
  t->type = type;
  map_resize(t, MINTABLESIZE);
  return t;
}

static void map_destroy(qmap *map) {
  if (map->length) {
    MapType freeMode = map->type;
    Type keytype = map_keytype(map);
    int entrysize = size_entry(map);
    if (keytype) {
      qentry_dict *entry2, *ptr;
      Type valtype;
      if (freeMode & MAP_FREE_VAL)
        qassert(valtype = map->valtype);
      for (int i = 0; i < map->size; i++) {
        entry2 = map->entry[i].dict;
        while (entry2) {
          ptr = entry2;
          entry2 = entry2->next;
          if (freeMode & MAP_FREE_KEY) {
            keytype->free(ptr->key);
          }
          if (freeMode & MAP_FREE_VAL) {
            valtype->free(ptr->val);
          }
          Mem.alloc(ptr, entrysize, 0);
        }
      }
    } else {
      if (map->type & MAP_TABLE)
        entrysize = sizeof(qentry_dict);
      else
        entrysize = sizeof(qentry_set);
      qentry_dict *entry1, *ptr;
      for (int i = 0; i < map->size; i++) {
        entry1 = map->entry[i].dict;
        while (entry1) {
          ptr = entry1;
          entry1 = entry1->next;
          if (freeMode & MAP_FREE_KEY) {
            ptr->key->type->free(ptr->key->val.i);
          }
          if (freeMode & MAP_FREE_VAL) {
            ptr->val->type->free(ptr->val->val.i);
          }
          Mem.alloc(ptr, entrysize, 0);
        }
      }
    }
  }
  Mem.alloc(map->entry, map->size * sizeof(qentry), 0);
  Mem.alloc(map, size_map, 0);
}

struct apiMap Map = {.create=map_new, map_resize, map_gset, map_del, map_iter,
                     map_next, map_destroy};
