#ifndef LINKLIST_H_INCLUDED
#define LINKLIST_H_INCLUDED
#ifdef __cplusplus
extern "C"{
#endif
#include "qlibc/base.h"
#include "stdint.h"
#define ll_tail(l) l->tail
#define ll_head(l) l->head
#define ll_data(n) (n)->data
#define ll_iter(l) cast(lNode,l)
#define ll_get(l,t) cast(t,ll_data(l))
#define ll_getTail(l,t) ll_get(ll_tail(l),t)
#define ll_getHead(l,t) ll_get(ll_head(l),t)
//使it指向下一个位置，并返回指向下一个位置后的迭代器
#define ll_next(l,t) (t=t->next)==ll_iter(l)?NULL:t
#define ll_prev(l,t) (t=t->prev)==ll_iter(l)?NULL:t
#define ll_empty(l) (l->head==l->tail)
#define ll_notempty(l) (l->head!=l->tail)
#define LL_NOT_FREE 0
#define LIST_FORECE_FREE 1
#define listType void*
typedef struct lnode {
	struct lnode *next;
	struct lnode *prev; //指向当前结点的上一结点
	listType data; //数据域指针
}*lNode,lnode;
//list为空时head和tail指向本身
typedef struct linklist {
	struct lnode *head;
	struct lnode *tail; //指向当前结点的上一结点
	intptr_t length; //链表list的长度
}*qlist;

struct apiList {
	//创建链表
	qlist (*create)();
#define ll_newnode(l,data) LL.newnode(l, cast(listType,data))
	lNode (*newnode)(qlist l, listType data);
	//把data的内容插入到链表list的末尾
#define ll_append(l,data) LL.append(l, cast(listType,data))
	lNode (*append)(qlist linklist, listType data);

	//把data的内容插入到链表的迭代器it_before的前面
	//assign指定数据data间的赋值方法
#define ll_insert(l,data) LL.insert(l, cast(listType,data))
	lNode (*insert)(qlist linklist, listType data);
	void (*linkNodeToPrev)(qlist linklist, lNode data, lNode lnode);

	//删除链表list中node指向的结点
	bool (*remove)(qlist linklist, lNode node);

	//返回list中第index个数据的指针
	lNode (*at)(qlist linklist, int index);

	//销毁list
	void (*destroy)(qlist *list_ptr, void (*destructor)(listType));

#define ll_push_back(l,data) LL.push_back(l, cast(listType,data))
	lNode (*push_back)(qlist l, listType data);

#define ll_push_front(l,data) LL.push_front(l, cast(listType,data))
	lNode (*push_front)(qlist l, listType data);
	listType (*pop_back)(qlist l);
	listType (*pop_front)(qlist l);
	qlist (*addArray)(qlist l, intptr_t *data, int n);
};
//LinkList
extern struct apiList List;
#ifdef __cplusplus
}
#endif
#endif // LIST_H_INCLUDED
