/*
 * control.c
 *
 *  Created on: 2019年2月21日
 *      Author: WanQing
 */
#include "qlibc/control.h"
#include "std/string.h"
#include "qlibc/mem.h"
#include "stl/map.h"
static gl_state qgs = { 0 };
static State qs = { &qgs };
State *_S = &qs;
int skyc_try(State* s, Pfunc f, void *ud) {
	struct longjmp jmp;
	jmp.prev = s->errorJmp;
	s->errorJmp = &jmp;
	if (setjmp(jmp.b) == 0)
		(*f)(s, ud);
	s->errorJmp = jmp.prev;
	return jmp.status;
}
void skyc_throw(State *s, errcode code, char *msg) {
	if (s->errorJmp) {
		s->errorJmp->status = code;
		longjmp(s->errorJmp->b, 1);
	} else if (s->errf) {
		s->errf(s, code, msg);
	} else if (s->up) {
		skyc_throw(s->up, code, msg);
	} else if (s->g && s->g->errf) {
		s->g->errf(s, code, msg);
	} else {
		sky_error(msg, code);
	}
}
void skyc_runerror(State *s, char *msg) {
	skyc_throw(s, ERR_RUN, msg);
}
void skyc_debug(intptr_t stopCondition) {
	if (stopCondition) {
		printf("stop for debug \n");
	}
}
void initcache() {
	gl_state *g = _S->g;
	skytstr_resize(_S, MINSTRTABSIZE);
	qstr *memerrmsg = STR.create(" ", 1);
	Mem.protect(_S, o2gc(memerrmsg));
	for (int i = 0; i < STRCACHE_N; i++) /* fill cache with valid strings */
		for (int j = 0; j < STRCACHE_M; j++)
			g->strcache[i][j] = memerrmsg;
}
extern void initType();

void skyc_init() {
	static bool isInit = false;
	if (isInit) {

	} else {
		initcache();
		initType();
#ifdef MEM_DEBUG
//		extern qmap *mrecord;
//		mrecord = Map.create(_S, typeInt,true);
		Mem.loadMeminfo();
#endif
		isInit = true;
	}
}
extern void rb_cache_clear();
extern void link_cache_clear();
extern void bytes_cache_clear();
extern void list_cache_clear();
extern void strt_destroy();
void skyc_destroy() {
	rb_cache_clear();
	link_cache_clear();
	bytes_cache_clear();
	list_cache_clear();
	strt_destroy();
}
const struct ctrlApi CTRL = { skyc_init, skyc_try, skyc_throw, skyc_runerror,
		skyc_debug, skyc_destroy };
