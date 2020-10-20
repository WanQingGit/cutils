/*
 * bytes.c
 *
 *  Created on: Jun 27, 2019
 *  Author: WanQing<1109162935@qq.com>
 */

#include "qlibc/stl/bytelist.h"

int main(){
	qbytes *bytes=Bytes.create(1);
	for(int i=0;i<5;i++){
		Bytes.push(bytes,&i);
	}
	Bytes.destroy(&bytes);
	bytes=Bytes.create(2);
	for(int i=0;i<5;i++){
		Bytes.push(bytes,&i);
	}
	Bytes.destroy(&bytes);
	return 0;
}
