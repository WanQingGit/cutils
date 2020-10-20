/*
 * logtest.c
 *
 *  Created on: Jul 4, 2019
 *  Author: WanQing<1109162935@qq.com>
 */

#include "utils/qlogger.h"
#include "std/string.h"
int main() {
//	char temp[]="nisk";
//	fprintf(fp,"ksk\n");
//	fprintf(fp,temp);
	qlogger *logger = Log.createByPath("res/logtest.txt");
	logger->policy|=LOG_CONSOLE;
	Log.log(logger, "hello");
	Log.flush(logger);
	Log.log(logger, "1234");
	Log.destroy(logger);
	logger = Log.create(stdout,LOG_ACTIVE);
	Log.log(logger,"this is a log test!");
	Log.destroy(logger);
	return 0;
}
