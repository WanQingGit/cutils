#ifndef SeqList_H_INCLUDED
#define SeqList_H_INCLUDED
#ifdef __cplusplus
extern "C"{
#endif
#include "cutils/base.h"
#include "cutils/object.h"
#define ARR_NOT_FREE 0
#define ARR_FORCE_FREE 1
#define ARR_TYPE_FREE 2
#define arr_type void*
#define arr_tail(l)  (l)->data[(l)->length-1]
#define arr_data(l,t,i) cast(t,l->next[i].p)
struct apiVec {
	qvec (*create)(int cap);
	qvec (*clone)(qvec list);
	qvec (*resize)(qvec list, int nsize);
	//把data的内容插入到链表list的末尾
	qvec (*append)(qvec list, arr_type data);
	qvec (*push)(qvec list, arr_type data);
	//把data的内容插入到链表的的前面
//	ArrayList  (*insert)(ArrayList list, arr_type data);

//删除链表list中指向的结点
	qvec (*remove)(qvec list, int index);

//删除链表list的第0个结点，下标从0开始
//	qval (*pop_front)(ArrayList list);

//删除链表list的最后一个结点
#define arr_pop(list,t) cast(t,Arr.pop(list))
	arr_type (*pop)(qvec list);
//	qval (*pop_back)(ArrayList list);

//返回list中第index个数据的指针
	arr_type (*at)(qvec list, size_t index);

//查找list中第一个与data相等的元素的下标，
//equal函数，当第一个参数与第二个参数的值相等时，返回1，否则返回0
//效率比较低,所以废除
//	int (*indexof)(ArrayList list, arr_type data, int (*equal)(arr_type v1, arr_type v2));

//销毁list
	void (*destroy)(qvec *list, void (*destructor)(arr_type v));
	qvec (*addArray)(qvec list, void *a, size_t n);
	qvec (*addFromVec)(qvec list, qvec a);
	qvec (*sort)(qvec l, compare f);
	qvec (*add)(qvec l, int index, arr_type data);
	qvec (*addSort)(qvec l, qval data,compare cmp, bool keepSame);
	void (*shrink)(qvec l);
	qtuple* (*toTuple)(qvec list);
};
extern struct apiVec Arr;
#ifdef __cplusplus
}
#endif
#endif // LIST_H_INCLUDED
