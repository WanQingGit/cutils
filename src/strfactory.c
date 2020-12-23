/*
 * qstrbuffer.c
 *
 *  Created on: Apr 11, 2019
 *  Author: WanQing<1109162935@qq.com>
 */

#include <std/string.h>
#include <stl/vector.h>
#include "cutils/stl/map.h"
#include <string.h>
#include "strutils.h"
#include "mem.h"

#define TEMP_FLAG -456
static qmap *strtable;

static qstr *string_createlstr(size_t l, const char *str);

static qstr *string_get(const char *str);

static qstr *newstr(size_t l, unsigned int h,
                    const char *str);

static qstr *string_new(const char *str, size_t l);


static void string_table_resize(size_t newsize) {
  Map.resize(strtable, newsize);
}

struct TemmStr {
    struct qstr s;
    char *val;
};

static qstr *gshrstr(const char *str, size_t l) {
  struct TemmStr tempStr;
  qstr *ts;
  gl_state *g = _G;
  unsigned int h = str_hash_count(str, l, g->seed);
  tempStr.s.hash = h;
  tempStr.val = str;
  tempStr.s.len = TEMP_FLAG;
  qentry qentry1;
  bool find = Map.gset(strtable, &tempStr, true, &qentry1);
  if (find) {
    map_get_key(qentry1,ts);
    return ts;
  }
  ts = newstr(l, h, str);
  incr_ref(ts);
  map_set_key(qentry1,ts);
//  GCNode *obj=o2gc(ts);
//  obj->nref--;
//  if(obj->nref<=0){
//    assert(obj->nref==0);
//    obj->type->free(obj+1);
//    qfree(obj);
//  }
  return ts;
}

static qstr *string_get(const char *str) {
  return string_new(str, strlen(str));
}

static qstr *string_new(const char *str, size_t l) {
  if (l <= MAXSHORTLEN)
    return gshrstr(str, l);
  else {
    qstr *ts = string_createlstr(l, str);
    return ts;
  }
}

static bool string_del(qstr *str) {
  qentry_dict entry;
  return Map.del(strtable, str, &entry);
}

static qstr *string_createlstr(size_t l, const char *str) {
  qstr *ts = newstr(l, 0, str);
  return ts;
}

static qstr *newstr(size_t l, unsigned int h,
                    const char *str) {
  size_t totalsize = sizeof(qstr) + (l + 1) * sizeof(char);
  qstr *ts = cast(qstr*, Mem.new(typeString, totalsize));
  ts->hash = h;
  ts->len = l;
  memcpy(gstr(ts), str, l * sizeof(char));
  gstr(ts)[l] = '\0';
  return ts;
}

static void strt_destroy() {
  Map.destroy(strtable);
}

static size_t str_comare(qstr *s1, qstr *s2) {
  size_t diff = s1->hash - s2->hash;
  if (diff) {
    return diff;
  }
  char *str1;
  if (s1->len == TEMP_FLAG)
    str1 = ((struct TemmStr *) (s1))->val;
  else
    str1 = s1->val;
  char *str2;
  if (s2->len == TEMP_FLAG)
    str2 = ((struct TemmStr *) (s2))->val;
  else
    str2 = s2->val;
  return strcmp(str1, str2);
}

static void str_free(qstr *str) {
  struct GCNode *obj = o2gc(str);
  Mem.alloc(obj, obj->size, 0);
}

static void strt_init() {
  Type typeLstr = (Type) (&STR2);
  memcpy(typeLstr, typeString, sizeof(struct typeobj));
  typeLstr->free = str_free;
  typeLstr->compare = str_comare;
  strtable = Map.create(typeLstr, MAP_FREE_KEY);
  Map.resize(strtable, MINSTRTABSIZE);
}

static size_t strt_size() {
  return strtable->length;
}

struct QString STR2 = {.init_env=strt_init, .destroy=strt_destroy, .create=string_new, .get=string_get, .strt_resize=string_table_resize, .size=strt_size};
