/*
 * qlogger.h
 *
 *  Created on: Jul 4, 2019
 *  Author: WanQing<1109162935@qq.com>
 */

#ifndef INCLUDE_QLOGGER_H_
#define INCLUDE_QLOGGER_H_
#ifdef __cplusplus
extern "C"{
#endif

#include <cutils/strutils.h>
#include "cutils/object.h"
typedef struct qlogger {
    qstrbuf buf;
    int length;
    int cachesize;
    int sum;
    FILE *stream;
    LogPolicy policy;
} qlogger;
struct apiLog {
	qlogger* (*create)(FILE *stream, LogPolicy policy);
	qlogger* (*createByPath)(char *path);
	void (*destroy)(qlogger* logger);
	void (*log)(qlogger* logger, char *msg);
	void (*add)(qlogger* logger, char *msg, int len);
	void (*flush)(qlogger* logger);
};
extern const struct apiLog Log;
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QLOGGER_H_ */
