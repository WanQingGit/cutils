#include <cutils/stl/list.h>
#include "cutils/mem.h"
#define LinkNodeToTail(list, data, node) LinkNodeToPrev(list, data, node->next)
#define MAXFREELIST 64
#define MAXFREENODE MAXFREELIST*4
#define list_freeNode(node)  \
		if (numfreenode < MAXFREENODE) { \
			free_node[numfreenode] = node; \
			numfreenode++;	\
			} else	\
			Mem.alloc( node, sizeof(*node), 0);
static qlist free_list[MAXFREELIST];
static lNode free_node[MAXFREENODE];
static int numfreelist = 0;
static int numfreenode = 0;

static qlist link_create();

//把data的内容插入到链表list的末尾
//assign指定数据data间的赋值方法
static lNode link_append(qlist list, void *data);

//把data的内容插入到链表的迭代器it_before的前面
//assign指定数据data间的赋值方法
static lNode link_insert(qlist list, void *data);
static lNode link_at(qlist list, int index);
static lNode link_newnode(qlist list, void* data);
static void linkNodeToPrev(qlist list, lNode data, lNode node);
static bool link_remove(qlist list, lNode node);

/***
 函数功能：初始化链表,数据域所占内存的大小由data_size给出
 ***/
static qlist link_create() {
	qlist l;
	if (numfreelist > 0) {
		numfreelist--;
		l = free_list[numfreelist];
	} else {
		l = (qlist) Mem.alloc( NULL, 0, sizeof(struct linklist));
	}
	l->length = 0;
	l->head = ll_iter(l);
	l->tail = ll_iter(l);
	return l;
}
/*
 * 函数功能：把data的内容插入到链表list的末尾
 * LinkNodeToPrev(list, node, list->head);
 */
static lNode link_append(qlist list, void *data) {
	lNode node = link_newnode(list, data);
	lNode tail = ll_tail(list);
	tail->next = node;
	node->prev = tail;
	node->next = cast(lNode, list);
	ll_tail(list) = node;
	++list->length;
	return node;
}
/*
 * 函数功能：把data的内容插入到链表list的迭代器it_before的前面
 */
static lNode link_insert(qlist list, void *data) {
	lNode node = link_newnode(list, data);
	linkNodeToPrev(list, node, ll_head(list));
//	node->prev = NULL;
	return node;
}
//next_node=list->tail
/***
 函数功能：把data连接到node之前
 ***/
static void linkNodeToPrev(qlist list, lNode data, lNode node) {
	lNode prev_node = node->prev;
	prev_node->next = data;
	node->prev = data;
	data->next = node;
	data->prev = prev_node;
	++list->length;
}
/***
 函数功能：从list中，移除node结点，但并不free
 注意，并不free结点，只是把结点从链中分离
 ***/
static bool link_remove(qlist list, lNode node) {
	if (node == ll_iter(list))
		return false;    //不移除头结点
	lNode next_node = node->next;
	lNode prev_node = node->prev;
	//使结点node从list中分离
	next_node->prev = prev_node;
	prev_node->next = next_node;
	//分享后，list的长度减1
	--list->length;
	return true;
}
/***
 函数功能：返回第index个结点的指针
 ***/
static lNode link_at(qlist list, int index) {
	lNode node = NULL;
	int len = list->length;
	qassert(index < len && index >= -len);
	if (index < 0)
		index += len;
	int i = 0;
	//如果index比长度的一半小，则从前往后找
	if (index <= len >> 1) {
		//设置node初值
		node = ll_head(list);
		for (i = 0; i < index; ++i) {
			node = node->next;    //向后移一个结点
		}
	}
	//否则从后往前找
	else {
		node = cast(lNode, list);    //设置node初值
		for (i = list->length; i > index; --i) {
			node = node->prev;    //向前移一个结点
		}
	}
	return node;
}

static lNode link_newnode(qlist l, void* data) {
	lNode node;
	if (numfreenode > 0) {
		numfreenode--;
		node = free_node[numfreenode];
	} else {
		node = (lNode) Mem.alloc( NULL, 0, sizeof(struct lnode));
	}
	node->data = data;
	return node;
}

/**
 * 函数功能：销毁链表list
 */
static void link_destroy(qlist *list_ptr, void (*destructor)(void*)) {
	qlist l = *list_ptr;
	if (l == NULL)
		return;
	if (l->length) {
		lNode tmp = NULL; //用于保存被free的结点的下一个结点
		lNode node = ll_head(l);
		while (l->length--) {
			qassert(node != ll_iter(l));
			tmp = node->next;
			if (destructor && node->data) {
				if ((size_t)destructor == LIST_FORECE_FREE)
					qfree(node->data);
				else
					destructor(node->data);
			}
			list_freeNode(node);
			node = tmp;
		}
	}
	if (numfreelist < MAXFREELIST) {
		free_list[numfreelist] = l;
		numfreelist++;
	} else {
		Mem.alloc( l, sizeof(*l), 0);
	}
	*list_ptr = NULL;
}

static listType link_pop_back(qlist l) {
	lNode n = ll_tail(l);
	if (link_remove(l, n)) {
		listType data=n->data;
		list_freeNode(n);
		return data;
	}
	return NULL;
}
static listType link_pop_front(qlist l) {
	lNode n = ll_head(l);
	if (link_remove(l, n)) {
		listType data=n->data;
		list_freeNode(n);
		return n;
	}
	return NULL;
}
static qlist link_addArray(qlist l, intptr_t *data, int n) {
	for (int i = 0; i < n; i++) {
		link_append(l, cast(void*, data[i]));
	}
	return l;
}
void link_cache_clear() {
	for (int i = 0; i < numfreelist; i++) {
		Mem.alloc( free_list[i], sizeof(struct linklist), 0);
	}
	numfreelist = 0;
	for (int i = 0; i < numfreenode; i++) {
		Mem.alloc( free_node[i], sizeof(lnode), 0);
	}
	numfreenode = 0;
}
struct apiList List = { link_create, link_newnode, link_append, link_insert,
		linkNodeToPrev, link_remove, link_at, link_destroy, link_append,
		link_insert, link_pop_back, link_pop_front, link_addArray }; //链表
