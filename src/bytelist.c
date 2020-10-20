#include <stl/bytelist.h>
#include "mem.h"
#include "control.h"
#include "std/string.h"
#include <string.h>
#define MAXFREELIST 64
#define DEFAULT_SIZE  8
static qbytes *free_list[MAXFREELIST];
static int numfreelist = 0;
static byte *bytes_at(qbytes *list, size_t index);
static qbytes *bytes_resize(qbytes *list, int nsize);
static qbytes *bytes_create(int datasize) {
	qbytes *l;
	if (numfreelist > 0) {
		numfreelist--;
		l = free_list[numfreelist];
	} else {
		l = (qbytes*) skym_alloc_pool(_S, NULL, 0, sizeof(struct qbytes));
	}
	if (datasize < 1)
		datasize = 1;
	l->data = (byte*) skym_alloc_pool(_S, NULL, 0, datasize * DEFAULT_SIZE);
	l->length = 0;
	l->datasize = datasize;
	l->capacity = DEFAULT_SIZE;
	return l;
}
static qbytes *bytes_append(qbytes *list, void *data) {
	if (list->length >= list->capacity) {
		bytes_resize(list, -1);
	}
	memcpy(Bytes_get(list, list->length,void), data, list->datasize);
	list->length++;
	return list;
}
static qbytes *bytes_resize(qbytes *list, int nsize) {
	if (nsize < 0) {
		int used = list->length;
		if (used >= list->capacity) {
			sky_assert(used == list->capacity);
			if (used < 64)
				nsize = used * 2;
			else {
				nsize = used * 1.5;
			}
			if (nsize < 4)
				nsize = 4;
			if (nsize < used)
				skyc_runerror(_S, "qbytes *overflow");
		} else {
			return list;
		}
	}
	int unitsize = sizeof(byte) * list->datasize;
	list->data = (byte*) skym_alloc_pool(_S, list->data,
			unitsize * list->capacity, unitsize * nsize);
	list->capacity = nsize;
	return list;
}

static qbytes *bytes_add(qbytes *l, int index, void *data) {
	int used = l->length;
	if (index < 0) {
		index += l->length;
		sky_assert(index >= 0);
	}
	sky_assert(index <= used);
	if (used >= l->capacity)
		bytes_resize(l, -1);
//	byte *ptr = l->data;
	for (; used > index; used--) {
		memcpy(Bytes_get(l, used,void), Bytes_get(l, used - 1,void), l->datasize);
//		ptr[used] = ptr[used - 1];
	}
	memcpy(Bytes_get(l, index,void), data, l->datasize);
//	ptr[index] = data;
	l->length++;
	return l;
}

qbytes *bytes_remove(qbytes *list, int index) {
	int used = list->length;
	sky_assert(index < used && index >= -used);
	if (index < 0)
		index += used;
	used -= 1;
	for (int i = index; i < used; i++) {
		memcpy(Bytes_get(list, i,void), Bytes_get(list, i + 1,void), list->datasize);
//		data[i] = data[i + 1];
	}
	list->length--;
	if ((list->length < list->capacity / 4) && (list->capacity > 8))
		bytes_resize(list, list->capacity / 2);
	return list;
}

static byte* bytes_at(qbytes *l, size_t index) {
	int used = l->length;
	sky_assert(index < used && index >= -used);
	if (index < 0)
		return Bytes_get(l, index + used,byte);
//		return l->data[index + used];
	return Bytes_get(l, index,byte);
}
static qbytes *bytes_addArray(qbytes *list, void *a, size_t n) {
	int used = list->length;
	int nused = used + n;
	if (nused >= list->capacity) {
		bytes_resize(list, nused * 1.2);
		sky_check(list->capacity <= nused, "qbytes *overflow!");
	}
	byte *data = Bytes_get(list, used,byte);
	byte *ba = cast(byte*, a);
	for (int i = 0; i <= n; i++) {
		memcpy(data + i * list->datasize, ba + i * list->datasize, list->datasize);
//		data[i] = a[i];
	}
	list->length = nused;
	return list;
}
static qbytes *bytes_addFromVec(qbytes *list, qbytes *a) {
	return bytes_addArray(list, a->data, a->length);
}

/***
 函数功能：销毁链表list
 ***/
static void bytes_destroy(qbytes **bytes_ptr) {
	qbytes *l = *bytes_ptr;
	skym_alloc_pool(_S, l->data, sizeof(byte) * l->datasize * l->capacity, 0);
	if (numfreelist < MAXFREELIST) {
		free_list[numfreelist] = l;
		numfreelist++;
	} else {
		skym_alloc_pool(_S, l, sizeof(qbytes), 0);
	}
	*bytes_ptr = NULL;
	return;
}
static byte* pop_back(qbytes *list) {
	if (list->length > 0) {
		list->length--;
		if ((list->length < list->capacity / 4) && (list->capacity > 16))
			bytes_resize(list, list->capacity / 2);
		return Bytes_get(list, list->length,byte);
	}
	return NULL;
}
static void bytes_shrink(qbytes *list) {
	list->data = (byte*) skym_alloc_pool(_S, list->data,
			sizeof(byte) * list->datasize * list->capacity,
			sizeof(byte) * list->datasize * list->length);
	list->capacity = list->length;
}
void bytes_cache_clear() {
	for (int i = 0; i < numfreelist; i++) {
		skym_alloc_pool(_S, free_list[i], sizeof(struct qbytes), NULL);
	}
	numfreelist = 0;
}
struct apiBytes Bytes = { bytes_create, bytes_resize, bytes_append,
		bytes_remove, pop_back, bytes_at, bytes_destroy, bytes_addArray,
		bytes_addFromVec, bytes_add, bytes_shrink };
