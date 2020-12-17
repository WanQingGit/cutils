/*
 * list.c
 *
 *  Created on: Apr 24, 2019
 *      Author: WanQing
 */

#include "cutils/stl/vector.h"


#include "cutils/control.h"
#include "cutils/stl/bytelist.h"
int main() {
	CTRL.init();
	qvec l = Arr.create(0);
	l->type = typeInt;
	for (int i = 1; i < 10; i++)
		Arr.append(l, i);
	qbytes *bytes=Bytes.create(1);
	typeList->serialize(bytes,l);
	Arr.destroy(&l, ARR_NOT_FREE);
	typeList->deserial(bytes->data,&l);
	for (int i = 0; i < l->length; i++){
		printf("%d\n",l->data[i].int32);
	}
	Bytes.destroy(&bytes);
	Arr.destroy(&l, ARR_NOT_FREE);
	return 0;
}
