/*
 * iotest.c
 *
 *  Created on: Jul 7, 2019
 *  Author: WanQing<1109162935@qq.com>
 */
#include "cutils/utils//io.h"
#include "cutils/prop.h"
#include "cutils/std/string.h"
#include "cutils/strutils.h"
#include "cutils/control.h"
int main() {
//	char temp[]="  \t\nsk \ts,\t\v\n\r ";
//	char *s=str_trim(temp);
//	printf("%s\n",s);
	CTRL.init();
	qprop *prop = Prop.read("res/parser.conf", "=");
	qstr *dirCode = Prop.get(prop, STR.get("DIR_CODE"), NULL);
	printf("DIR_CODE=%s\n", dirCode->val);
	Prop.destroy(prop);
	return 0;
}
