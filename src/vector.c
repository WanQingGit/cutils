#include <stl/vector.h>
#include <cutils/stl/bytelist.h>
#include <cutils/utils/io.h>
#include "cutils/mem.h"
#include "cutils/control.h"
#include "std/string.h"

#define MAXFREELIST 64
#define DEFAULT_SIZE  8
static qvec free_list[MAXFREELIST];
static int numfreelist = 0;

static void *list_at(qvec list, ssize_t index);

static qvec list_resize(qvec list, int nsize);

static qvec list_create(Type type, int cap, ArrMode mode) {
  qvec l;
  if (cap == 0)
    cap = DEFAULT_SIZE;
  if (numfreelist > 0) {
    numfreelist--;
    l = free_list[numfreelist];
    l->length = 0;
  } else {
    l = (qvec) Mem.alloc(NULL, 0, sizeof(struct qvector));
  }
  l->type = type;
  size_t datasize;
  if (mode & ARR_DATA) {
    assert (type && type->size > 0);
    datasize = type->size;
  } else
    datasize = sizeof(void *);
  l->datasize = datasize;
  l->data = Mem.alloc(NULL, 0, datasize * cap);
  l->capacity = cap;
  l->mode = mode;
  if (type)
    l->assign = type->assign;
  return l;
}

static qvec _list_append(qvec list, void *data) {
  if (list->length >= list->capacity) {
    list_resize(list, -1);
  }
  if (list->mode & ARR_DATA) {
    void *dst = ((char *) list->data) + list->length * list->datasize;
    if (list->assign)
      list->assign(data, dst);
    else {
      memcpy(dst, data, list->datasize);
    }
  } else {
    list->data[list->length] = (size_t) data;
  }

  list->length++;
  return list;
}

static qvec list_resize(qvec list, int nsize) {
  if (nsize < 0) {
    int used = list->length;
    if (used >= list->capacity) {
      qassert(used == list->capacity);
      if (used < 64)
        nsize = used * 2;
      else {
        nsize = used * 1.5;
      }
      if (nsize < 4)
        nsize = 4;
      if (nsize < used)
        qrunerror(_S, "qvec overflow");
    } else {
      return list;
    }
  }
  list->data = Mem.alloc(list->data, list->datasize * list->capacity,
                         list->datasize * nsize);
  list->capacity = nsize;
  return list;
}

static qvec add(qvec l, int index, void *data) {
  int used = l->length;
  if (index < 0) {
    index += l->length;
    qassert(index >= 0);
  }
  qassert(index <= used);
  if (used >= l->capacity)
    list_resize(l, -1);
  char *ptr = (char *) l->data;
  size_t datsize = l->datasize;
  if (datsize >= sizeof(void *)) {
    memcpy(ptr + used * datsize, ptr + (used - 1) * datsize, datsize * (used - index));
  } else {
    for (; used > index; used--) {
      memcpy(ptr + used * datsize, ptr + (used - 1) * datsize, datsize);
//      ptr[used] = ptr[used - 1];
    }
  }
  if (l->assign)
    l->assign(ptr + index * datsize, data);
  else
    memcpy(ptr + index * datsize, data, datsize);
//  ptr[index] = data;
  l->length++;
  return l;
}

static qvec addSort(qvec l, void *data, bool keepSame) {
  comparefun cmp = l->compare;
  int used = l->length;
  int i, mid, low = 0, high = used - 1;
  if (l->mode & ARR_DATA) {
    char *ptr = (char *) l->data;
    size_t datasize = l->datasize;
    while (mid <= high) {
      mid = (low + high) / 2;
      i = cmp(data, ptr + datasize * mid);
      if (!keepSame && i == 0)
        return l;
      if (i > 0)
        low = mid + 1;
      else
        high = mid - 1;
    }
  } else {
    size_t *arrdata = (size_t *) l->data;
    while (mid <= high) {
      mid = (low + high) / 2;
      i = cmp(data, (void *) arrdata[mid]);
      if (!keepSame && i == 0)
        return l;
      if (i > 0)
        low = mid + 1;
      else
        high = mid - 1;
    }
  }

  mid = (low + high) / 2;
  add(l, mid, data);
  return l;
}

//void moveArray(ListS list)
qvec list_remove(qvec list, int index) {
  int used = list->length;
  qassert(index < used && index >= -used);
  if (index < 0)
    index += used;
  qval *data = list->data;
  used -= 2;
  for (int i = index; i < used; i++) {
    data[i] = data[i + 1];
  }
  list->length--;
  if ((list->length < list->capacity / 4) && (list->capacity > 8))
    list_resize(list, list->capacity / 2);
  return list;
}

static void *list_at(qvec l, ssize_t index) {
  int used = l->length;
  qassert(index < used && index >= -used);
  if (index < 0)
    index += used;
  if (l->mode & ARR_DATA)
    return (char *) l->data + index * l->datasize;
  return (void *) l->data[index];
}

static qvec list_addArray(qvec list, void *a, size_t n) {
  int used = list->length;
  int nused = used + n;
  size_t datasize = list->datasize;
  if (nused > list->capacity) {
    list_resize(list, nused * 1.2);
    qcheck(list->capacity <= nused, "qvec overflow!");
  }
  char *dst = list->data + used * datasize;
  memcpy(dst, a, n * datasize);
//  for (int i = 0; i <= n; i++) {
//    data[i] = a[i];
//  }
  list->length = nused;
  return list;
}

static qvec list_addFromVec(qvec list, qvec a) {
  int used = list->length;
  int nused = used + a->length;
  if (nused >= list->capacity) {
    list_resize(list, nused * 1.2);
    qcheck(list->capacity >= nused, "qvec overflow!");
  }
//  qval *data = list->data + used;
//  int n = a->length;
//  for (int i = 0; i < n; i++) {
//    data[i] = a->data[i];
//  }
  size_t datasize = list->datasize;
  void *dst = (char *) list->data + used * datasize;
  void *src = (char *) a->data;
  int n = a->length;
  memcpy(dst, src, n * datasize);
  list->length = nused;
  return list;
}

static qvec sort(qvec l) {
//	int j;
//	int used = l->used;
//	compare f = l->cmp;
//	InsertS(l, cast(qval, NULL));
//	for (int i = 2; i < used; i++) {
//		if (f(l->head[i], l->head[i - 1]) < 0) {
//			j = i;
//			l->head[0] = l->head[i];
//			while (f(l->head[--j], l->head[0]) > 0) {
//				l->head[j + 1] = l->head[j];
//			}
//			l->head[j + 1] = l->head[0];
//		}
//	}
//	list_remove(l, 0);
  return l;
}

/***
 函数功能：销毁链表list
 ***/
static void list_destroy(qvec *list_ptr) {
  qvec l = *list_ptr;
  qval *data = l->data;
  int used = l->length;
  ArrMode mode = l->mode;

  if (mode & ARR_PTR) {
    if (mode & ARR_FREE_FORCE) {
      for (int i = 0; i < used; i++)
        Mem.alloc(data[i].p, sizeof(qval), 0);
    } else if (mode & ARR_FREE_TYPE) {
      for (int i = 0; i < used; i++)
        l->type->free(data[i].p);
    } else if (mode & ARR_TYPE) {
//      for (int i = 0; i < used; i++)
//        destructor(data[i]);
    }
  }
  Mem.alloc(data, sizeof(qval) * l->capacity, 0);
  if (numfreelist < MAXFREELIST) {
    free_list[numfreelist] = l;
    numfreelist++;
  } else {
    Mem.alloc(l, sizeof(struct qvector), 0);
  }
  *list_ptr = NULL;
}

static bool pop_back(qvec list, size_t *data) {
  if (list->length > 0) {
    list->length--;
    if (list->mode & ARR_DATA) {
      size_t datasize = list->datasize;
      if ((list->length < list->capacity / 4) && (list->capacity > 16))
        list_resize(list, list->capacity / 2);
      memcpy(data, (char *) list->data + list->length * datasize, datasize);
    } else {
      *data = list->data[list->length];
    }
    return true;
  }
  return false;
}

static void list_shrink(qvec list) {
  list->data = Mem.alloc(list->data, list->datasize * list->capacity,
                         list->datasize * list->length);
  list->capacity = list->length;
}


static qvec list_clone(qvec list) {
  qvec newlist = list_create(list->type, list->length, list->mode);
  if (list->length) {
    memcpy(newlist->data, list->data, list->length * list->datasize);
    newlist->length = list->length;
  }
  return newlist;
}

void list_cache_clear() {
  for (int i = 0; i < numfreelist; i++) {
    Mem.alloc(free_list[i], sizeof(struct qvector), NULL);
  }
  numfreelist = 0;
}

static int init_env() {

}

uint hashList(intptr_t o) {
  return HASHMASK & o;
}

void serialList(qbytes *bytes, void *o) {
  checksize(bytes, sizeof(struct QVectorApi));
  qvec list = cast(qvec, o);
  size_t len = list->length;
  byte *l = Bytes_tail(bytes, byte);
  writeWord(l, len);
  writeWord(l, list->mode);
  bytes->length = l - bytes->data;
  typeType->serialize(bytes, list->type);
  l = Bytes_tail(bytes, byte);
//	bytes->length += 4;
  if (len) {
    if (list->type) {
      Type type = list->type;
      for (int i = 0; i < len; i++) {
        type->serialize(bytes, (void *) list->data[i]);
      }
    } else {
      checksize(bytes, len * sizeof(void *));
      l = Bytes_tail(bytes, byte);
      writeBytes(l, list->data, len * sizeof(void *));
      bytes->length += len * sizeof(void *);
    }
  }
}

int deserialList(byte *l, intptr_t *i) {
  byte *ol = l;
  size_t len = readWord(l);
  size_t mode = readWord(l);
  Type type;
  l += typeType->deserial(l, &type);
  qvec list = Arr.create(type, len, mode);
  if (len) {
    if (type) {
      for (int k = 0; k < len; k++) {
        l += type->deserial(l, i);
        Arr.append(list, *i);
      }
    } else {
      memcpy(list->data, l, len * sizeof(void *));
      list->length = len;
      l += len * sizeof(void *);
      //		Arr.addArray(list,l,len);
      //		for (int k = 0; k < len; k++) {
      //			Arr.append(list, readInt64(l));
      //		}
    }
  }
  *i = list;
  return l - ol;
}

struct QVectorApi Arr = {.type={.serialize=serialList, .deserial=deserialList, .free=list_destroy, .id=V_ARRAY, .size=sizeof(qvector), .name="<vector>"},
        .create=list_create, list_clone, list_resize, _list_append, _list_append,
                             list_remove, pop_back, list_at, list_destroy, list_addArray, list_addFromVec, sort, add,
                             addSort, list_shrink};
const Type typeList = &Arr.type;