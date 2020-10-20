/*
 * iotest.c
 *
 *  Created on: Jul 7, 2019
 *  Author: WanQing<1109162935@qq.com>
 */
#include "qlibc/utils//io.h"
#include "qlibc/prop.h"
#include "qlibc/std/string.h"
#include "qlibc/strutils.h"
#include "qlibc/control.h"
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
