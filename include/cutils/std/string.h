/*
 * qstrbuffer.h
 *
 *  Created on: Apr 11, 2019
 *      Author: WanQing
 */

#ifndef INCLUDE_QSTRUTILS_H_
#define INCLUDE_QSTRUTILS_H_
#ifdef __cplusplus
extern "C"{
#endif

#include "cutils/base.h"
#include "cutils/object.h"
#include <string.h>
#define MINSTRTABSIZE	 128

struct strApi {
	qstr* (*create)(const char *str, size_t l);
	qstr *(*get)(const char *str);
	void (*strt_resize)(State *S, int newsize);
	void (*add)(qstrbuf *buffer, const char *s, int n);
	/*
	 * 将字符串s中的p替换成r,例如s="a,b" p="," r="_" 最后结果是"a_b"
	 * */
	void (*sub)(qstrbuf *buffer, const char *s, const char *p, const char *r);
	void (*split)(qvec l, const char *str, const char *s);
	int (*index)(qstrbuf *buffer, const char *s);
};
void string_table_resize(State *S, int newsize);
extern struct strApi STR;
//#define STR_NEW (qstrbuf *)calloc(1,sizeof(qstrbuf))
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QSTRUTILS_H_ */
