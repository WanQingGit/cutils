/*
 * object.h
 *
 *  Created on: 2019年2月21日
 *      Author: WanQing
 */

#ifndef INCLUDE_OBJECT_H_
#define INCLUDE_OBJECT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "cutils/base.h"
#include "cutils/conf.h"
#include "setjmp.h"

typedef enum {
    ERR_NO, ERR_MEM, ERR_RUN, ERR_ASSERT
} errcode;

/*type of variable*/
typedef enum {
    V_NIL,
    V_FUNCTION,
    V_STR,
    V_NUMFLT,
    V_NUMINT,
    V_BOOL,
    V_TABLE,
    V_ARRAY,
    V_RBTREE,
    V_TYPE,
    V_USERDATA,
} qtype;
#define HASHMASK (~((size_t)0))
typedef struct qhashMap qmap;
typedef struct qobj qobj;
typedef struct qvector *qvec;
typedef struct qstr qstr;
typedef struct qbytes qbytes;
#define OBJ(v, t) {{cast(void*,v)},t}

#define o2gc(o) (cast(GCObj*,o)-1)
#define incr_ref(o) o2gc(o)->nref++

typedef void *(*errfun)(void *u, errcode code, char *msg);

typedef size_t (*hashfun)(const qobj *o);

typedef void (*serialfun)(qbytes *l, void *o);

typedef int (*deserialfun)(byte *l, intptr_t *o);

typedef size_t (*comparefun)(void *a, void *b);

typedef char *(*o2strfun)(qbytes *l, qobj *o);

typedef void (*freefun)(qobj *o);

#define sizearr(x) (1<<(x))
typedef struct typeobj {
    comparefun compare; //同类型比较函数
    hashfun hash;
    serialfun serialize;
    deserialfun deserial;
    o2strfun toString;
    freefun free;
    size_t id;
    char name[16];
} typeobj, *Type;
//extern const  uint HASHMASK = -1;
#define TYPEDEFINE \
 const Type typeType, typeInt, typeFloat, typeBool, typeString, typeMap,\
        typeList, typeModule, typeCFun, typeCClosure, typeClosure, typeRBTree,\
        typeThread, typeNULL;
extern TYPEDEFINE;

typedef enum {
    RED, BLACK
} rbcolor;
typedef enum {
    RB_NEAR, RB_BELOW, RB_ABOVE
} rb_flag;
typedef struct RBNode {
    intptr_t key;                    // key和val的位置不能改变
    intptr_t val;
    struct RBNode *left;
    struct RBNode *right;
    struct RBNode *parent;
    rbcolor color: 8;        // 颜色(RED 或 BLACK)
} RBNode;
//typedef int (*comparef)(rbtype *a, rbtype *b);
// 红黑树的根
typedef struct RBTree {
    Type typeKey;
    Type typeVal;
    RBNode *root;
    int length;
    bool multi: 8;
} RBTree;
typedef struct GCObj {
    struct GCObj *next;
    ssize_t nref;
    typeobj *type;
    ssize_t size;
} GCObj;
struct qstr {
    struct qstr *hnext;
    size_t hash;
    int len;
    char val[];
};

struct qbytes {
    int length;
    int capacity;
    byte *data;
    int datasize;
};

typedef struct stringtable {
    qstr **ht;
    uint nuse; /* number of elements */
    uint size;
} stringtable;
typedef struct gc {
    GCObj *allgc; /* list of all collectable objects */
    GCObj *protect; /* list of all collectable objects */
    ptrdiff_t GCdebt; /* bytes allocated not yet compensated by the collector */
} GC;
struct longjmp {
    struct longjmp *prev;
    jmp_buf b;
    volatile errcode status;
};

typedef struct glstate {
    GC gc;
    stringtable strt;
    RBTree *typeinfos;
    errfun errf;
    uint seed;
} gl_state;
typedef struct qstate {
    gl_state *g;
    struct qstate *up;
    struct longjmp *errorJmp;
    errfun errf;
} State;

typedef union qval {
    void *p;
    qmap *t;
    qvec arr;
    qstr *s;
    INT i;
    FLT flt;
    qobj *obj;
    signed int32: 32;
    float flt32;
} qval;
typedef struct qobj {
    typeobj *type;
    qval val;
} qobj;
/**
 * qentry_dict,4用于节省内存，使用keytype保存类型
 * qentry1,2,3,4的next和key的位置一定要一致
 * 不一致遍历会出错
 * 1,2用于map,3,4用于set
 * qentry的key除非能保证hash和compare和原key相同
 * 不然一定不要更改，切记！
 */
 /**
  * deprecated
  * 没有必要存储类型信息
  */
//typedef struct qentry1 {
//    struct qentry1 *next;
//    qobj key;
//    qobj val;
//} qentry1;

//typedef struct qentry3 {
//    struct qentry3 *next;
//    qobj key;
//} qentry3;

typedef struct qentry_dict {
    struct qentry_dict *next;
    qobj *key;
    qobj *val;
} qentry_dict;


typedef struct qentry_set {
    struct qentry_set *next;
    qobj *key;
} qentry_set;
typedef union qentry {
//    qentry1 *_1;
    qentry_set *set;
    qentry_dict *dict;
} qentry;
/**
 * 第一位用于判断是否是table
 * 第二位用于判断是否使用keytype
 * map创建后不要修改该值
 */
typedef enum {
    MAP_SET = 0, MAP_TABLE = 1, MAP_OBJTYPE = 2,MAP_FREE_VAL=4,MAP_FREE_KEY=8,MAP_FREE_FORCE=16
} MapType;
/**
 * 当存放的对象不是qobj对象时，使用keytype来实现增删改查
 * map一旦创建，keytype不能修改，valtype可修改,所以keytype不可见
 * keytype不为null,则插入的key必须满足该类型，插入的值是原生不带类型的。
 * keytype为null,则值必须带类型。
 * valtype用于序列化，仅当keytype存在时有效
 */
struct qhashMap {
    qentry *entry;
    Type valtype;
    uint size;
    uint length;
    MapType type;
#ifdef QDEBUG
    uint nfilled;
#endif
};
//typedef struct qdict {
//    qmap ht[2];
//    byte state;
//} qdict;
extern qobj nilobj;

typedef int (*qfunc)(State *S, int argc, int resc);

typedef int (*compare)(qval v1, qval v2);

typedef struct qvector {
    int length;
    int capacity;
    qval *data;
    Type type;
} qvector;
typedef struct {
    int length;
    qval data[];
} qtuple;

typedef struct qstrbuffer {
    uint size;
    uint n; //used
    char *val;
} qstrbuf;
typedef enum {
    LOG_LAZY = 0, LOG_ACTIVE = 1 << 2, LOG_NORMAL = 1 << 3, LOG_CONSOLE = 1 << 4
} LogPolicy;
typedef struct qlogger {
    qstrbuf buf;
    int length;
    int cachesize;
    int sum;
    FILE *stream;
    LogPolicy policy;
} qlogger;

extern State *_S;

Type createType(char *name, comparefun compare, hashfun hash,
                serialfun serialize, deserialfun deserial, o2strfun toString, freefun free,
                size_t baseType);

bool destroyType(qstr *name);

#define typeCompare(compare) createType(NULL, cast(comparefun,compare), NULL, NULL, NULL, NULL, NULL, V_NIL)
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_OBJECT_H_ */
