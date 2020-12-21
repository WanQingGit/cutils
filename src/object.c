/*
 * qobject.c
 *
 *  Created on: May 22, 2019
 *  Author: WanQing<1109162935@qq.com>
 */

#include "cutils/object.h"
#include "utils/io.h"
#include "stl/map.h"
#include "std/string.h"
#include "stl/vector.h"
#include "stl/rbtree.h"
#include "stl/bytelist.h"
#include "cutils/control.h"
#include <math.h>
//TYPEDEFINE
void serialStr(qbytes *l, qstr *s);
int serialInt(qbytes *bytes, INT o);
int deserialInt(byte *l, intptr_t *i);

void serialType(qbytes *l, Type o) {
  serialInt(l,o->id);
//	serialStr(l, o->name);
}
int deserialStr(byte *l, intptr_t *i);
int deserialType(byte *l, intptr_t *i) {
//	qstr *s;
//	int diff = deserialStr(l, &s);
	int diff=deserialInt(l,i);
//	l += diff;
	qentry entry;
	RBNode *node = RB.search(_S->g->typeinfos, *i);
	*i = node->val;
	return diff;
}
size_t cmpInt(INT a, INT b) {
	return a - b;
//	if (b->type == typeInt)
//		return a - b->val.i;
//	else if (b->type == typeFloat)
//		return cmpFloat(cast(FLT, a), b);
//	return -1;
}
size_t hashInt(INT o) {
	return o;
}

int serialInt(qbytes *bytes, INT o) {
	checksize(bytes, sizeof(INT));
	*Bytes_tail(bytes, INT) = o;
	bytes->length += sizeof(INT);
}
int deserialInt(byte *l, intptr_t *i) {
	*i = *cast(INT*, l);
	return sizeof(INT);
}
char* int2str(qbytes* bytes, INT o) {
	checksize(bytes, 20); //long max
	char *ptr = Bytes_tail(bytes, char);
	bytes->length += sprintf(ptr, "%d", o);
	return ptr;
}
INT cmpFloat(FLT a, FLT b) {
	static FLT eps_0 = 1.0e-6;
	if (fabs(a - b) < eps_0)
		return 0;
	return a > b ? 1 : -1;
}

size_t hashFloat(const INT o) {
	return cast(size_t, o);
}

void serialFlt(qbytes *bytes, FLT o) {
	checksize(bytes, sizeof(FLT));
	*Bytes_tail(bytes, FLT) = o;
	bytes->length += sizeof(FLT);
}
int deserialFlt(byte *l, FLT *i) {
	*i = *cast(FLT*, l);
	return sizeof(FLT);
}

char* flt2str(qbytes* bytes, FLT o) {
	checksize(bytes, 15);
	char *ptr = Bytes_tail(bytes, char);
	bytes->length += sprintf(ptr, "%f", o);
	return ptr;
}

size_t hashBool(const qobj *o) {
	return o->val.i ? 1 : 0;
}

void serialBool(qbytes *bytes, intptr_t o) {
	checksize(bytes, 1);
	*Bytes_tail(bytes, byte) = (o != 0);
	bytes->length++;
}
int deserialBool(byte *l, bool *i) {
	*i = *cast(byte*, l);
	return sizeof(byte);
}
char* bool2str(qbytes* bytes, bool o) {
	checksize(bytes, 4);
	char *ptr = Bytes_tail(bytes, char);
	if (o) {
		writeBytes(ptr, "true", 4);
	} else
		writeBytes(ptr, "false", 4);
	bytes->length += 4;
	return ptr - 4;
}
static size_t cmpStr(qstr *s1, qstr *s2) {
  size_t diff = s1->hash - s2->hash;
  if (diff) {
    return diff;
  }
  return strcmp(s1->val,s2->val);
}
//size_t cmpStr(qstr *a, qstr *b) {
//	return strcmp(a->val, b->val);
//}
size_t hashStr(const qstr *o) {
	return o->hash;
}
void serialStr(qbytes *l, qstr *s) {
	int len = s->len + 5;
	checksize(l, len);
	byte *ptr = Bytes_tail(l, byte);
	writeInt32(ptr, s->len);
	writeBytes(ptr, s->val, s->len + 1);
	l->length += len;
}
int deserialStr(byte *l, intptr_t *i) {
	byte *ol = l;
	int len = readInt32(l);
	char *val = qmalloc2(char, len+1);
	readBytes(val, l, len + 1);
	qstr *s = STR.create(val, len);
	*i = s;
  qfree(val);
	return l - ol;
}
char* str2str(qbytes* bytes, qstr *o) {
	int len = o->len;
	checksize(bytes, len);
	char *ptr = Bytes_tail(bytes, char);
	writeBytes(ptr, o->val, len);
	bytes->length += len;
	return ptr - len;
}
uint hashMap(qmap *map) {
//	bool val = Map.gset(_S, map, STR.get("_hash_"), false, NULL);
//	if (val) {
//		return 0;
//	} else {
	return HASHMASK & cast(uintptr_t, map);
//	}
}
void serialMap(qbytes *bytes, qmap *map) {
	mapIter iter;
	Map.iterator(map, &iter);
	Type keytype = map_keytype(map);
	Type valtype = map->valtype;
	byte *l = Bytes_push(bytes, 10, byte);
	writeInt32(l, map->length); //4
	writeInt32(l, map->size); //4
	writeByte(l, map->type); //1
	if (valtype)
		qassert(map->type & MAP_TABLE);
	if (keytype) {
		writeByte(l, 1);
		typeType->serialize(bytes, keytype);
	} else
		writeByte(l, 0);
	l = Bytes_add(bytes, byte);
	if (valtype) {
		writeByte(l, 1);
		typeType->serialize(bytes, valtype);
	} else
		writeByte(l, 0);
	if (keytype) {
		if (valtype) {
			while (Map.next(&iter)) {
				keytype->serialize(bytes, iter.entry.dict->key);
				valtype->serialize(bytes, iter.entry.dict->val);
			}
		} else {
			while (Map.next(&iter)) {
				keytype->serialize(bytes, iter.entry.dict->key);
			}
		}
	} else {
		if (map->type & MAP_TABLE) {
			while (Map.next(&iter)) {
				qobj *key = iter.entry.dict->key;
				qobj *val = iter.entry.dict->val;
				key->type->serialize(bytes, key->val.i);
				val->type->serialize(bytes, val->val.i);
			}
		} else {
			while (Map.next(&iter)) {
				qobj *key = &iter.entry.dict->key;
				key->type->serialize(bytes, key->val.i);
			}
		}
	}
}

int deserialMap(byte *l, intptr_t *map_ptr) {
	byte *ol = l;
	Type keytype = NULL, valtype = NULL;
	int length = readInt32(l);
	int size = readInt32(l);
	int type = readByte(l);
	int typeflag = readByte(l);
	if (typeflag) {
		l += typeType->deserial(l, &keytype);
	}
	qmap *map = Map.create(keytype, type & MAP_TABLE);
	*map_ptr = map;
	typeflag = readByte(l);
	if (typeflag) {
		l += typeType->deserial(l, &valtype);
		map->valtype = valtype;
	}
	Map.resize(map, size);
	INT key, value;
	qentry entry;
	if (keytype) {
		if (valtype) {
			for (int i = 0; i < length; i++) {
				l += keytype->deserial(l, &key);
				l += valtype->deserial(l, &value);
				qassert(Map.gset(map, key, true, &entry) == false);
				entry.dict->val = value;
			}
		} else {
			for (int i = 0; i < length; i++) {
				l += keytype->deserial(l, &key);
				qassert(Map.gset(map, key, true, &entry) == false);
			}
		}
	} else {
		if (map->type & MAP_TABLE) {
			for (int i = 0; i < length; i++) {
				l += typeType->deserial(l, &keytype);
				l += keytype->deserial(l, &key);
				l += typeType->deserial(l, &valtype);
				l += valtype->deserial(l, &value);
				qassert(Map.gset(map, key, true, &entry) == false);
				entry.dict->val = value;
			}
		} else {
			for (int i = 0; i < length; i++) {
				l += typeType->deserial(l, &keytype);
				l += keytype->deserial(l, &key);
				qassert(Map.gset(map, key, true, &entry) == false);
			}
		}
	}
	return l - ol;
}
uint hashList(intptr_t o) {
	return HASHMASK & o;
}
void serialList(qbytes *bytes, void *o) {
	checksize(bytes, 5);
	qvec list = cast(qvec, o);
	int len = list->length;
	int n = sizeof(list->length);
	byte *l = Bytes_tail(bytes, byte);
	writeInt32(l, len);
	bytes->length += 4;
	if (len) {
		bytes->length += 1;
		if (list->type) {
			writeByte(l, 1);
			serialStr(bytes, list->type->name);
			Type type = list->type;
			for (int i = 0; i < len; i++) {
				type->serialize(bytes, list->data[i].obj);
			}
		} else {
			writeByte(l, 0);
			checksize(bytes, len * 8);
			l = Bytes_tail(bytes, byte);
			writeBytes(l, list->data, len * 8);
			bytes->length += len * 8;
//			for (int i = 0; i < len; i++) {
//				l = writeInt64(l, list->data[i].i);
//			}
		}
	}
}
int deserialList(byte *l, intptr_t *i) {
	byte *ol = l;
	int len = readInt32(l);
	qvec list = Arr.create(len);
	if (len) {
		bool typeFlag = readByte(l);
		if (typeFlag) {
			l += deserialStr(l, i);
			qstr *typename = cast(qstr*, *i);
			RBNode *node = RB.search(_S->g->typeinfos, typename);
			qassert(node && node->val);
			Type type = cast(Type, node->val);
			list->type = type;
			for (int k = 0; k < len; k++) {
				l += type->deserial(l, i);
				Arr.append(list, *i);
			}
		} else {
			memcpy(list->data, l, len * 8);
			list->length = len;
			l += len * 8;
			//		Arr.addArray(list,l,len);
			//		for (int k = 0; k < len; k++) {
			//			Arr.append(list, readInt64(l));
			//		}
		}
	}

	*i = list;
	return l - ol;
}

INT cmpPtr(intptr_t a, intptr_t b) {
	return a - b;
}
uint hashPtr(intptr_t i) {
	return HASHMASK & i;
}

void serialRB(qbytes *bytes, RBTree *t) {
	Type typeKey = t->typeKey;
	Type typeVal = t->typeVal;
	serialType(bytes, typeKey);
	checksize(bytes, 5);
	byte *ptr = Bytes_tail(bytes, byte);
	bytes->length += 5;
	writeInt32(ptr, t->length);
	RBNode *node = RB.min(t);
	if (typeVal) {
		writeByte(ptr, 1);
		serialType(bytes, typeVal);
		while (node) {
			typeKey->serialize(bytes, node->key);
			typeVal->serialize(bytes, node->val);
			node = RB.next(node);
		}
	} else {
		writeByte(ptr, 0);
		while (node) {
			typeKey->serialize(bytes, node->key);
			node = RB.next(node);
		}
	}
}
int deserialRB(byte *l, intptr_t *i) {
	RBTree *tree;
	byte *ol = l;
	intptr_t x, y;
	l += deserialStr(l, i);
	int len = readInt32(l);
	qstr *typename = cast(qstr*, *i);
	RBNode *node = RB.search(_S->g->typeinfos, typename);
	qassert(node && node->val);
	Type typeKey = cast(Type, node->val);
	int valFlag = readByte(l);
	if (valFlag) {
		l += deserialStr(l, i);
		typename = cast(qstr*, *i);
		node = RB.search(_S->g->typeinfos, typename);
		qassert(node && node->val);
		Type typeVal = cast(Type, node->val);
		tree = RB.create(typeKey, typeVal);
		for (int k = 0; k < len; k++) {
			l += typeKey->deserial(l, &x);
			l += typeVal->deserial(l, &y);
			qassert(RB.insert(tree,x,y,NULL));
		}
	} else {
		for (int k = 0; k < len; k++) {
			l += typeKey->deserial(l, &x);
			qassert(RB.insert(tree,x,NULL,NULL));
		}
	}
	*i = tree;
	return l - ol;
}
/*type=qmalloc(typeobj);\
		 type->baseType=base;\
		 type->name=STR.get("<"#str">");\
		 type->compare=comparef;\
		 type->serialize=serial;\
		 type->deserial=deser;\
		 type->hash=hashfun;\
		 type->free=freef;\
		 type->toString=strf;\*/
// @formatter:off
static typeobj typeBase[] = {
{       cmpInt,    hashInt,   serialInt, deserialInt, int2str,  NULL, V_NUMINT ,"<int>",}, //0
{   cmpFloat,  hashFloat, serialFlt, deserialFlt, flt2str,  NULL, V_NUMFLT ,"<float>" }, //1
{      cmpInt,    hashBool,  serialBool,deserialBool,bool2str, NULL, V_BOOL ,"<bool>",}, //2
{    cmpStr,    hashStr,   serialStr, deserialStr, str2str,  NULL, V_STR ,"<string>",}, //3
{       NULL,      hashMap,   serialMap, deserialMap,	NULL,     NULL, V_TABLE ,"<map>",}, //4
{      NULL,      hashList,  serialList,deserialList,NULL,     NULL, V_ARRAY ,"<list>",}, //5
{  cmpPtr,    hashPtr,   NULL,      NULL,        NULL,     NULL, V_FUNCTION ,"<function>"}, //6
{    cmpPtr,    hashPtr,   serialRB,  deserialRB,  NULL,     NULL, V_RBTREE ,"<RBTree>"}, //7
{        cmpPtr,    hashPtr,   NULL,      NULL,        NULL,     NULL, V_NIL ,"null",}, //8
{     cmpPtr,    hashPtr,   serialType,deserialType,NULL,     NULL, V_TYPE,"<type>"} //9
};

const Type typeInt = &typeBase[0],
					 typeFloat = &typeBase[1],
					 typeBool =&typeBase[2],
					 typeString = &typeBase[3],
					 typeMap = &typeBase[4],
					 typeList =&typeBase[5],
					 typeCFun = &typeBase[6],
					 typeRBTree =&typeBase[7],
					 typeNULL = &typeBase[8],
					 typeType=&typeBase[9];
// @formatter:on
//#define TYPEREG(type,name,hashfun,serial,deserial,comparef,strf,freef,base) \
//  static typeobj _##type={"<"#name">",hashfun,serial,deserial,comparef,strf,freef,base};\
//  const Type type=& _##type;
//TYPEREG(typeInt, 		int,    hashInt,   serialInt, deserialInt, NULL, NULL, NULL, V_NUMINT)
//TYPEREG(typeFloat,  float,  hashFloat, serialFlt, deserialFlt, NULL, NULL, NULL, V_NUMFLT)
//TYPEREG(typeBool,   bool,   hashBool,  serialBool,deserialBool,NULL, NULL, NULL, V_BOOL)
//TYPEREG(typeString, string, hashStr,   serialStr, deserialStr, NULL, NULL, NULL, V_STR)
//TYPEREG(typeMap,    map,    hashMap,   NULL,		  NULL,		     NULL, NULL, NULL, V_TABLE)
//TYPEREG(typeList,   list,   hashList,  serialList,deserialList,NULL, NULL, NULL, V_ARRAY)
//TYPEREG(typeModule, module, hashPtr,   NULL,      NULL,        NULL, NULL, NULL, V_MODULE)
//TYPEREG(typeCFun, cfunction,hashPtr,   NULL,      NULL,        NULL, NULL, NULL, V_LCF)
//TYPEREG(typeCClosure,cclosure,hashPtr, NULL,      NULL,        NULL, NULL, NULL, V_CCL)
//TYPEREG(typeClosure,function, hashPtr, NULL,      NULL,        NULL, NULL, NULL, V_LCL)
//TYPEREG(typeRBTree, RBTree, hashPtr,   NULL,      NULL,        NULL, NULL, NULL, V_RBTREE)
//TYPEREG(typeThread, thread, hashPtr,   NULL,      NULL,        NULL, NULL, NULL, V_THREAD)
//TYPEREG(typeNULL,   null,   hashPtr,   NULL,      NULL,        NULL, NULL, NULL, V_NIL)
static INT charcmp(qstr *a, qstr *b) {
	return strcmp(a->val, b->val);
}
void initType() {
	int len = sizeof(typeBase) / sizeof(typeBase[0]);
//	Type cmp = typeCompare(charcmp);
	RBTree* tree = RB.create(NULL, NULL);
	_S->g->typeinfos = tree;
	for (int i = 0; i < len; i++) {
		Type t = &typeBase[i];
//		char *name = cast(char*, t->name);
//		t->name = STR.create(name, strlen(name));
		qassert(RB.insert(tree,(rbtype)t->id,t,NULL));
	}
}
bool destroyType(qstr *name) {
	RBNode *node = RB.search(_S->g->typeinfos, name);
	if (node == NULL)
		return false;
	Type t = cast(Type, node->val);
	RB.delNode(_S->g->typeinfos, node);
	qfree(t);
	return true;
}
Type createType(char *name, comparefun compare, hashfun hash,
		serialfun serialize, deserialfun deserial, o2strfun toString, freefun free,
		size_t typeid) {
	Type t = qmalloc(typeobj);
	if (name) {
		qassert(RB.insert(_S->g->typeinfos,t->name,t,NULL));
	}
	t->compare = compare;
	t->hash = hash;
	t->serialize = serialize;
	t->deserial = deserial;
	t->toString = toString;
	t->free = free;
	t->id = typeid;
	return t;
}
