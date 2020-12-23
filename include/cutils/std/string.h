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


#define MINSTRTABSIZE   128
#define str_val(s) ((s)->val)
struct QString {
    typeobj type;

    void (*init_env)();

    void (*destroy)();

    qstr *(*create)(const char *str, size_t l);

    qstr *(*get)(const char *str);

    void (*strt_resize)(size_t newsize);

    size_t (*size)();
};

extern struct QString STR;
extern struct QString STR2;
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QSTRUTILS_H_ */
