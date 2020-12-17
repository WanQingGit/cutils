/*
 *
 *  Created on: Jul 4, 2019
 *  Author: WanQing<1109162935@qq.com>
 *
 *  为了减少函数调用，提高效率，代码会有冗余
 */
#include "utils/qlogger.h"
#include "std/string.h"
#include "cutils/mem.h"
#define LOG_BUFSIZE 2048
static void logger_log(qlogger* logger, char *msg);
static void log_flush(qlogger* logger);
#define checkconsole(logger)\
		if (logger->policy & LOG_CONSOLE)\
			fprintf(stdout, logger->buf.val)
static qlogger* log_create(FILE *stream, LogPolicy policy) {
	qlogger *logger = cast(qlogger *, Mem.alloc(_S, NULL, 0, sizeof(qlogger)));
	logger->stream = stream;
	logger->cachesize = LOG_BUFSIZE;
	logger->policy = policy;
	return logger;
}
static qlogger* log_createByPath(char *path) {
	FILE *fp = fopen(path, "w");
	qlogger *logger = cast(qlogger *, Mem.alloc(_S, NULL, 0, sizeof(qlogger)));
	logger->cachesize = LOG_BUFSIZE;
	logger->policy = LOG_NORMAL;
	logger->stream = fp;
	return logger;
}
static void log_destroy(qlogger* logger) {
	qstrbuf *buf = &logger->buf;
	char endinfo[] = "logger finished,the total out size is ";
	if (buf->size) {
		STR.add(buf, endinfo, sizeof(endinfo) - 1);
		STR.add(buf, endinfo, sprintf(endinfo, "%d\n", logger->sum));
		log_flush(logger);
	}
	fclose(logger->stream);
	qfree(logger->buf.val);
	Mem.alloc(_S, logger, sizeof(qlogger), 0);
}

static void logger_log(qlogger* logger, char *msg) {
	qstrbuf *buf = &logger->buf;
	int len = strlen(msg);
	STR.add(buf, msg, len);
	STR.add(buf, '\n', 0);
	logger->sum += len + 1;
	if (logger->policy == 0)
		return;
	else if (logger->buf.n > logger->cachesize || logger->policy & LOG_ACTIVE) {
		fprintf(logger->stream, buf->val);
		checkconsole(logger);
		buf->n = 0;
	}
}
static void logger_add(qlogger* logger, char *msg, int len) {
	qstrbuf *buf = &logger->buf;
	STR.add(buf, msg, len);
	logger->sum += len;
	if (logger->buf.n > logger->cachesize || logger->policy & LOG_ACTIVE) {
		fprintf(logger->stream, buf->val);
		checkconsole(logger);
		buf->n = 0;
	}
}
static void log_flush(qlogger* logger) {
	qstrbuf *buf = &logger->buf;
	if (buf->n) {
		fprintf(logger->stream, buf->val);
		checkconsole(logger);
		buf->n = 0;
	}
}
const struct apiLog Log = { log_create, log_createByPath, log_destroy,
		logger_log, logger_add, log_flush };
