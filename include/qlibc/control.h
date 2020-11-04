/*
 *
 *  Created on: 2019年2月21日
 *      Author: WanQing
 */

#ifndef INCLUDE_CONTROL_H_
#define INCLUDE_CONTROL_H_
#ifdef __cplusplus
extern "C"{
#endif
#include "qlibc/object.h"

#define qassert_(S,expr)							\
  ({			\
      if (!(expr))								\
    	 qthrow(S, ERR_ASSERT,"error:"#expr"\n");\
    })
#define qassert(expr)							\
  ({			\
      if (!(expr))								\
    	 qthrow(_S, ERR_ASSERT,"error:"#expr"\n");\
    })

#define skyc_error(msg) qthrow(_S, ERR_RUN,"error:"#msg"\n")
/* type of protected functions, to be ran by 'runprotected' */
typedef void (*Pfunc)(State *S, void *ud);
int qtry(State* s, Pfunc f, void *ud);
void qthrow(State *s, errcode code, char *msg);
void qrunerror(State *s, char *msg);

struct ctrlApi {
	void (*init)();
	int (*try)(State* s, Pfunc f, void *ud);
	void (*throw)(State *s, errcode code, char *msg);
	void (*runerror)(State *s, char *msg);
	void (*stopif)(intptr_t stopCondition);
	void (*destroy)();
};

extern const struct ctrlApi CTRL;

#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_CONTROL_H_ */
