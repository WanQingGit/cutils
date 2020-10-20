/*
 * IOUtil.h
 *
 *  Created on: Sep 11, 2018
 *      Author: WanQing
 */

#ifndef INCLUDE_QIO_H_
#define INCLUDE_QIO_H_
#ifdef __cplusplus
extern "C"{
#endif
#include<qlibc/base.h>

#define writeNormalVar(ptr,n,t) ({\
	t *_p=cast(t*,ptr);\
	*_p=n;\
	ptr=cast(typeof(ptr),_p+1);\
})
#define writeBytes(dst,src,n)	{\
	memcpy(dst,src,n);\
	dst=cast(typeof(dst),cast(char*,dst)+n);\
}
#define writeByte(ptr,n) writeNormalVar(ptr,n,byte)
#define writeInt16(ptr,n) writeNormalVar(ptr,n,short)
#define writeInt32(ptr,n) writeNormalVar(ptr,n,int)
#define writeInt64(ptr,n) writeNormalVar(ptr,n,long long)
#define writeFlt32(ptr,n) writeNormalVar(ptr,n,float)
#define writeFlt64(ptr,n) writeNormalVar(ptr,n,double)

#define readNormalVar(ptr,type) ({\
	ptr=cast(type*,ptr)+1;\
	*(cast(type*,ptr)-1);		\
})
#define readBytes(dst,src,n)  {\
	memcpy(dst,src,n);\
	src=cast(char*,src)+n;\
}

#define readByte(ptr)  readNormalVar(ptr,byte)
#define readInt16(ptr) readNormalVar(ptr,short)
#define readInt32(ptr) readNormalVar(ptr,int)
#define readInt64(ptr) readNormalVar(ptr,long long)
#define readFlt32(ptr) readNormalVar(ptr,float)
#define readFlt64(ptr) readNormalVar(ptr,double)
#define READ_SIZE 1024
typedef struct qfilereader{
	FILE *file;
//	int  ncache;
	char content[READ_SIZE];
}qreader;
qreader *reader_create(char *path);
char *file_read(char *path);
int read_line(qreader *reader);
void reader_destroy(qreader *reader);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QIO_H_ */
