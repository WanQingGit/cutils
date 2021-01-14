#ifndef SeqList_H_INCLUDED
#define SeqList_H_INCLUDED
#ifdef __cplusplus
extern "C"{
#endif

#include "cutils/base.h"
#include "cutils/object.h"


#define arr_type void*


typedef enum {
    ARR_PTR = 1, ARR_TYPE = 2, ARR_DATA = 4,
    ARR_FREE_FORCE = 8, ARR_FREE_TYPE = 16
} ArrMode;


typedef struct qvector {
    size_t length;
    size_t *data;
    size_t datasize;
    size_t capacity;
    Type type;
//    assignfunc assign;
//    comparefun compare;
    ArrMode mode;
} qvector;

typedef struct _qtuple {
    size_t length;
    size_t *data;
    size_t datasize;
}qtuple;

#define arr_tail(l)  (l)->data[(l)->length-1]
#define arr_data(l, t, i) cast(t,l->next[i].p)
struct QVectorApi {
    struct typeobj type;

    int (*init_env)();

    qvec (*create)(Type type, size_t cap,ArrMode mode);

    qvec (*clone)(qvec list);

    qvec (*resize)(qvec list, int nsize);

    //把data的内容插入到链表list的末尾
#define list_append(l,data) Arr.append((qvec)(l),(arr_type)(data))
    qvec (*append)(qvec list, arr_type data);

    qvec (*push)(qvec list, arr_type data);
    //把data的内容插入到链表的的前面
//	ArrayList  (*insert)(ArrayList list, arr_type data);

//删除链表list中指向的结点
    qvec (*remove)(qvec list, int index);

//删除链表list的第0个结点，下标从0开始
//	qval (*pop_front)(ArrayList list);

//删除链表list的最后一个结点
#define arr_pop(list, t) cast(t,Arr.pop(list))

    arr_type(*pop)(qvec list);
//	qval (*pop_back)(ArrayList list);

//返回list中第index个数据的指针
#define list_get(v,l,i) v=(typeof(v))((l)->data[i])
    arr_type(*at)(qvec list, ssize_t index);

//查找list中第一个与data相等的元素的下标，
//equal函数，当第一个参数与第二个参数的值相等时，返回1，否则返回0
//效率比较低,所以废除
//	int (*indexof)(ArrayList list, arr_type data, int (*equal)(arr_type v1, arr_type v2));

//销毁list
    void (*destroy)(qvec *list);

    qvec (*addArray)(qvec list, void *a, size_t n);

    qvec (*addFromVec)(qvec list, qvec a);

    qvec (*sort)(qvec l);

    qvec (*add)(qvec l, int index, arr_type data);

    qvec (*addSort)(qvec l, qval data, bool keepSame);

    void (*shrink)(qvec l);
};

extern struct QVectorApi Arr;
const extern Type typeList;

#define iter_list(l,val) for(size_t val##_i=0,val##_len=(l)->length;val##_i<val##_len&&((val=(typeof(val))(l)->data[val##_i])||1);val##_i++)

#define iter_list_data(l,val) for(size_t val##_i=0,val##_len=(l)->length,val##_datasize=(l)->datasize;val##_i<val##_len&&((val=(typeof(val))((char*)((l)->data)+val##_i*val##_datasize))||1);val##_i++)
#ifdef __cplusplus
}
#endif
#endif // LIST_H_INCLUDED
