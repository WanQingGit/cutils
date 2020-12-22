/*
 *  Created on: Apr 24, 2019
 *      Author: WanQing
 */

#include "cutils/stl/vector.h"


#include "cutils/control.h"
#include "cutils/stl/bytelist.h"
int main() {
	CTRL.init();
	qvec l = Arr.create(typeInt,0,ARR_TYPE);
	for (int i = 1; i < 100; i++)
		Arr.append(l, i);
	qbytes *bytes=Bytes.create(1);
	typeList->serialize(bytes,l);
	Arr.destroy(&l);
	typeList->deserial(bytes->data,(intptr_t *)&l);
	assert(l->length==99);
	for (int i = 0; i < l->length; i++){
	  assert(l->data[i]==(i+1));
		printf("%ld\n",l->data[i]);
	}
	Bytes.destroy(&bytes);
	Arr.destroy(&l);
	CTRL.destroy();
	return 0;
}
