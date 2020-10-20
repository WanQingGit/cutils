#ifndef INCLUDED_BYTES_H_
#define INCLUDED_BYTES_H_

#ifdef __cplusplus
extern "C"{
#endif
#include "qlibc/base.h"
#include "qlibc/object.h"

#define Bytes_get(list,index,t) cast(t*,(list)->data+(index)*(list)->datasize)
#define Bytes_tail(l,t) Bytes_get(l,l->length,t)
#define Bytes_last(l,t) Bytes_get(l,l->length-1,t)
#define checksize(l,n) \
if((l)->length+(n)>(l)->capacity){ \
		Bytes.resize((l),((l)->length+n)*1.7); \
}
#define Bytes_add(l,t) ({\
		checksize(l,1);l->length+=1;Bytes_last(l,t);})
#define Bytes_push(l,n,t) ({\
		checksize(l,n);l->length+=n;Bytes_get(l,l->length-n,t);})
#define correctLen(l,ptr) \
		sky_assert((ptr-l->data)%l->datasize)\
		l->length=(ptr-l->data)/l->datasize
struct apiBytes {
	//初始化链表
	qbytes *(*create)(int datasize);
	qbytes *(*resize)(qbytes *list, int nsize);
	//把data的内容插入到链表list的末尾
	qbytes *(*push)(qbytes *list, void *data);
	//把data的内容插入到链表的的前面
//	ArrayList  (*insert)(ArrayList list, void *data);

//删除链表list中指向的结点
	qbytes *(*remove)(qbytes *list, int index);

//删除链表list的第0个结点，下标从0开始
//	qval (*pop_front)(ArrayList list);

//删除链表list的最后一个结点
	byte* (*pop)(qbytes *list);
//	qval (*pop_back)(ArrayList list);

//返回list中第index个数据的指针
	byte* (*at)(qbytes *list, size_t index);

//查找list中第一个与data相等的元素的下标，
//equal函数，当第一个参数与第二个参数的值相等时，返回1，否则返回0
//效率比较低,所以废除
//	int (*indexof)(ArrayList list, void *data, int (*equal)(byte v1, byte v2));

//销毁list
	void (*destroy)(qbytes **list);
	qbytes *(*addArray)(qbytes *list, void *a, size_t n);
	qbytes *(*addFromVec)(qbytes *list, qbytes *a);
	qbytes *(*add)(qbytes *l, int index, void *data);
	void (*shrink)(qbytes *l);
};
extern struct apiBytes Bytes;

#ifdef __cplusplus
}
#endif
#endif // LIST_H_INCLUDED
