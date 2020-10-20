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

#define skyc_assert(S,expr)							\
  ({			\
      if (!(expr))								\
    	 skyc_throw(S, ERR_ASSERT,"error:"#expr"\n");\
    })
#define skyc_assert_(expr)							\
  ({			\
      if (!(expr))								\
    	 skyc_throw(_S, ERR_ASSERT,"error:"#expr"\n");\
    })

#define skyc_error(msg) skyc_throw(_S, ERR_RUN,"error:"#msg"\n")
/* type of protected functions, to be ran by 'runprotected' */
typedef void (*Pfunc)(State *S, void *ud);
int skyc_try(State* s, Pfunc f, void *ud);
void skyc_throw(State *s, errcode code, char *msg);
void skyc_runerror(State *s, char *msg);

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
