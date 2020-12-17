/*
 *
 *  Created on: Jul 7, 2019
 *  Author: WanQing<1109162935@qq.com>
 */

#ifndef INCLUDE_QPROP_H_
#define INCLUDE_QPROP_H_
#ifdef __cplusplus
extern "C"{
#endif
#include "cutils/stl/rbtree.h"

typedef struct {
	RBTree *props;
	char *src;
	qstr *sep;
} qprop;
qprop *prop_create(char *path, char *sep);
qstr *prop_get(qprop *prop, qstr *key, qstr* defval);
void prop_destroy(qprop *prop);
struct apiProp {
	qprop *(*read)(char *path, char *sep);
	void (*write)(qprop *prop);
	qstr *(*get)(qprop *prop, qstr *key, qstr* defval);
	void (*insert)(qprop *prop, qstr *key, qstr* val);
	void (*destroy)(qprop *prop);
};
extern const struct apiProp Prop;
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QPROP_H_ */
