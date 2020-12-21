#include <stl/vector.h>
#include "cutils/mem.h"
#include "cutils/control.h"
#include "std/string.h"
#define MAXFREELIST 64
#define DEFAULT_SIZE  8
static qvec free_list[MAXFREELIST];
static int numfreelist = 0;
static qval list_at(qvec list, size_t index);
static qvec list_resize(qvec list, int nsize);

static qvec list_create(int cap) {
	qvec l;
	if (cap == 0)
		cap = DEFAULT_SIZE;
	if (numfreelist > 0) {
		numfreelist--;
		l = free_list[numfreelist];
		l->length = 0;
	} else {
		l = (qvec) Mem.alloc( NULL, 0, sizeof(struct qvector));
	}
	l->data = (qval*) Mem.alloc( NULL, 0, sizeof(qval) * cap);
	l->capacity = cap;
	return l;
}

static qvec list_append(qvec list, qval data) {
	if (list->length >= list->capacity) {
		list_resize(list, -1);
	}
	list->data[list->length++] = data;
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
//	list->data = (qval*) realloc(list->data, sizeof(qval) * nsize);
	list->data = (qval*) Mem.alloc( list->data, sizeof(qval) * list->capacity,
                                   sizeof(qval) * nsize);
	list->capacity = nsize;
	return list;
}

static qvec add(qvec l, int index, qval data) {
	int used = l->length;
	if (index < 0) {
		index += l->length;
		qassert(index >= 0);
	}
	qassert(index <= used);
	if (used >= l->capacity)
		list_resize(l, -1);
	qval *ptr = l->data;
	for (; used > index; used--) {
		ptr[used] = ptr[used - 1];
	}
	ptr[index] = data;
	l->length++;
	return l;
}
static qvec addSort(qvec l, qval data, compare cmp, bool keepSame) {
	int used = l->length;
	int i, mid, low = 0, high = used - 1;
	while (mid <= high) {
		mid = (low + high) / 2;
		i = cmp(data, l->data[mid]);
		if (!keepSame && i == 0)
			return l;
		if (i > 0)
			low = mid + 1;
		else
			high = mid - 1;
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

static qval list_at(qvec l, size_t index) {
	int used = l->length;
	qassert(index < used && index >= -used);
	if (index < 0)
		return l->data[index + used];
	return l->data[index];
}
static qvec list_addArray(qvec list, qval *a, size_t n) {
	int used = list->length;
	int nused = used + n;
	if (nused > list->capacity) {
		list_resize(list, nused * 1.2);
		qcheck(list->capacity <= nused, "qvec overflow!");
	}
	qval *data = list->data + used;
	for (int i = 0; i <= n; i++) {
		data[i] = a[i];
	}
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
	qval *data = list->data + used;
	int n = a->length;
	for (int i = 0; i < n; i++) {
		data[i] = a->data[i];
	}
	list->length = nused;
	return list;
}

static qvec sort(qvec l, compare f) {
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
static void list_destroy(qvec *list_ptr, void (*destructor)(qval)) {
	qvec l = *list_ptr;
	qval *data = l->data;
	int used = l->length;
	if (destructor) {
		if (destructor == ARR_FORCE_FREE) {
			for (int i = 0; i < used; i++)
				Mem.alloc( data[i].p, sizeof(qval), 0);
		} else if (destructor == ARR_TYPE_FREE) {
			for (int i = 0; i < used; i++)
				l->type->free(data[i].p);
		} else {
			for (int i = 0; i < used; i++)
				destructor(data[i]);
		}
	}
    Mem.alloc( data, sizeof(qval) * l->capacity, 0);
	if (numfreelist < MAXFREELIST) {
		free_list[numfreelist] = l;
		numfreelist++;
	} else {
		Mem.alloc( l, sizeof(struct qvector), 0);
	}
	*list_ptr = NULL;
	return;
}
static qval pop_back(qvec list) {
	if (list->length > 0) {
		list->length--;
		if ((list->length < list->capacity / 4) && (list->capacity > 16))
			list_resize(list, list->capacity / 2);
		return list->data[list->length];
	}
	return cast(qval, 0);
}
static void list_shrink(qvec list) {
	list->data = (qval*) Mem.alloc( list->data, sizeof(qval) * list->capacity,
                                   sizeof(qval) * list->length);
	list->capacity = list->length;
}
static qtuple* list_toTuple(qvec list) {
	qtuple *tuple = (qtuple*) Mem.alloc( NULL, 0,
                                        sizeof(qval) * list->length + sizeof(qtuple));
	tuple->length = list->length;
	for (int i = 0; i < tuple->length; i++) {
		tuple->data[i] = list->data[i];
	}
	return tuple;
}
static qvec list_clone(qvec list) {
	qvec newlist = list_create(list->length);
	if (list->length) {
		memcpy(newlist->data, list->data, list->length * sizeof(qval));
		newlist->length = list->length;
	}
	return newlist;
}
void list_cache_clear() {
	for (int i = 0; i < numfreelist; i++) {
		Mem.alloc( free_list[i], sizeof(struct qvector), NULL);
	}
	numfreelist = 0;
}

struct apiVec Arr = { list_create, list_clone, list_resize, list_append,
		list_append, list_remove, pop_back, list_at,
		/*IndexOfS,*/list_destroy, list_addArray, list_addFromVec, sort, add,
		addSort, list_shrink, list_toTuple };
