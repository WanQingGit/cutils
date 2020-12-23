/*
 * StrUtil.c
 *
 *  Created on: Sep 11, 2018
 *      Author: WanQing
 */

#include <stl/map.h>
#include <time.h>
#include <std/string.h>
#include <cutils/stl/vector.h>
#include "cutils/strutils.h"
#include "cutils/mem.h"
//MASK_ALPHA	=1 MASK_DIGIT  =2 MASK_PRINT  =4
//MASK_SPACE  =8 MASK_XDIGIT =16
#define MKPR MASK_PRINT
#define MKAP MKPR|MASK_ALPHA //A-Za-z
#define MKDD MASK_DIGIT|MASK_PRINT //0-9
#define MKHD MKDD|MASK_XDIGIT	//0-9
#define MKHA MASK_XDIGIT|MKAP //A-F a-f
// @formatter:off
const typechar char_type[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  0
		0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, //  8
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 16
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 24
		0x0c, MKPR, MKPR, MKPR, MKPR, MKPR, MKPR, MKPR, // 32    ! " # $ % & '
		MKPR, MKPR, MKPR, MKPR, MKPR, MKPR, MKPR, MKPR, // 40	 ( ) * + , - . /
		MKHD, MKHD, MKHD, MKHD, MKHD, MKHD, MKHD, MKHD,//  48  0 1 2 3 4 5 6 7
		MKHD, MKHD, MKPR, MKPR, MKPR, MKPR, MKPR, MKPR, // 56	 8 9 : ; < = > ?
		MKPR, MKHA, MKHA, MKHA, MKHA, MKHA, MKHA, MKAP, // 64  @ A B C D E F G
		MKAP, MKAP, MKAP, MKAP, MKAP, MKAP, MKAP, MKAP,	// 72	 H I J K S M N O
		MKAP, MKAP, MKAP, MKAP, MKAP, MKAP, MKAP, MKAP, // 80  P Q R S T U V W
		MKAP, MKAP, MKAP, MKPR, MKPR, MKPR, MKPR, MKAP, // 88	 X Y Z [ \ ] ^ _
		MKPR,	MKHA, MKHA, MKHA, MKHA, MKHA, MKHA, MKAP, // 96  ` a b c d e f g
		MKAP, MKAP, MKAP, MKAP, MKAP, MKAP, MKAP, MKAP, //104	 h i j k l m n o
		MKAP,	MKAP, MKAP, MKAP, MKAP, MKAP, MKAP, MKAP, //112  p q r s t u v w
		MKAP, MKAP, MKAP, MKPR, MKPR, MKPR, MKPR, 0x00,//	120	 x y z { | } ~ DEL
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
// @formatter:on
uint makeseed(State *S) {
	char *buff[2];
	uint h = cast(uint, time(NULL));
	memcpy(buff, &S, sizeof(char*));
	memcpy(buff + 1, &h, sizeof(char*));
	return str_hash_count(cast(char *, buff), 2 * sizeof(char*), h);
}

size_t str_hash_count(const char *str, size_t l, uint seed) {
	unsigned int h = seed ^ cast(uint, l); //Times33
	size_t step = (l >> HASHLIMIT) + 1;
	for (; l >= step; l -= step)
		h ^= ((h << 5) + (h >> 2) + (byte) (str[l - 1]));
	return h;
}
char *str_trim(char *s) {
	if (is_space(*s)) {
		while (is_space(*(++s)))
			;
	}
	char* dst = s, *ptr;
	while (*dst) {
		if (is_space(*dst)) {
			ptr = dst;
			while (is_space(*(++dst)))
				;
			if (*dst == '\0') {
				*ptr = '\0';
				return s;
			}
		} else
			dst++;
	}
	return s;
}
/**
 * 没有从后面分析，因为调用strlen实际上从头到尾遍历了一遍
 */
char* str_rtrim(char* s) {
	char* dst = s, *ptr;
	while (*dst) {
		if (is_space(*dst)) {
			ptr = dst;
			while (is_space(*(++dst)))
				;
			if (*dst == '\0') {
				*ptr = '\0';
				return s;
			}
		} else
			dst++;
	}
	return s;
}
//bool empty(char* s) {
//	return *s == 0 ? true : false;
//}

void *qmemcpy(void *dst, const void *src, size_t num) {
	byte l = sizeof(size_t);
	qassert((dst!=NULL)&&(src!=NULL));
	int wordnum = num / l;
	int slice = num % l;
	size_t* pintsrc = (size_t *) src;
	size_t* pintdst = (size_t *) dst;
	char* cdst, *csrc;
	while (wordnum--)
		*pintdst++ = *pintsrc++;
	cdst = (char *) pintdst;
	csrc = (char*) pintsrc;
	while (slice--)
		*cdst++ = *csrc++;
	return dst;
}
char* ltrim(char* s) {
//	char* dst=s;
//	int offset=0;
	do {
		if (*s == ' ' || *s == '\t' || *s == '\n')
			continue;
		else
			break;
	} while (*++s);
//	*s='\0';
//	while(*dst++=*s++);同一块地址操作出错误
	return s;
}
char* skip_empty(char* s) {
	do {
		switch (*s) {
		case ' ':
		case '\f':
		case '\t':
		case '\v':
		case '\n':
		case '\r':
			break;
		default:
			return s;
		}
	} while (*++s);
	return s;
}
char skip_space(char** s) {
	do {
		switch (**s) {
		case ' ':
		case '\f':
		case '\t':
		case '\v':
			break;
		default:
			return **s;
		}
	} while (*(++*s));
	return **s;
}
char* strip(char* s) {
	size_t i;
	size_t len = strlen(s);
	size_t offset = 0;
	for (i = 0; i < len; ++i) {
		char c = s[i];
		if (c == ' ' || c == '\t' || c == '\n')
			++offset;
		else
			s[i - offset] = c;
	}
	s[len - offset] = '\0';
	return s;
}

//char* strcpyq(char *dst, char* s) {
//	char *strDest = dst;
////    assert((dst)!=NULL && (s)!=NULL);
////	while ((*dst++ = *s++)!='\0');
//	while ((*dst++ = *s++) != 0)
//		;
////	*dst='\0';
//	return strDest;
//}
//创建不清除原指针
//char* strcpym(char **dst, char* s) {
//	*dst = (char*) malloc(sizeof(char) * (strlen(s) + 1));
//	return strcpyq(*dst, s);
//}
bool numchar(char* s) {
	while (*s != '\0') {
		if ((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z')
				|| (*s >= '0' && *s <= '9') || *s == '_')
			++s;
		else
			return false;
	}
	return true;
}
char* backspace(char* s, char** cursor) {
	while (*cursor != s) {
		--*cursor;
		if (**cursor != '\t' && **cursor != ' ' && **cursor != '\n') {
			return *cursor;
		}
	};
	return *cursor;
}


static void qbufAdd(qstrbuf *buffer, const char *s, int n) {
  if (n >= 0) {
    if(n==0)
      n=(int)strlen(s);
    if (buffer->size - buffer->n < n) {
      uint nsize = (buffer->size + n) * 1.9;
      qassert(nsize > buffer->size);
      buffer->val = (char *) realloc(buffer->val, (nsize + 1) * sizeof(char));
      buffer->size = nsize;
    }
    memcpy(buffer->val + buffer->n, s, n * sizeof(char));
    buffer->n += n;
    buffer->val[buffer->n] = '\0';
  } else if (n == -1) {
    if (buffer->size == buffer->n) {
      uint nsize = (int) ((buffer->size + n) * 1.5);
      if (nsize < 4)
        nsize = 4;
      qassert(nsize > buffer->size);
      buffer->val = (char *) realloc(buffer->val, (nsize + 1) * sizeof(char));
      buffer->size = nsize;
    }
    buffer->val[buffer->n++] = cast(char, s);
  } else {
    qerror("cannot add a string of negative length", ERR_RUN);
  }
}

static void qsub(qstrbuf *buffer, const char *s, const char *p, const char *r) {
  char *ptr;
  int len = strlen(p), rlen = strlen(r);
  while ((ptr = strstr(s, p)) != NULL) {
    qbufAdd(buffer, s, ptr - s);
    qbufAdd(buffer, r, rlen);
    s = ptr + len;
  }
  qbufAdd(buffer, s, strlen(s));
}

static int qindex(qstrbuf *buffer, const char *s) {
  char *ptr = strstr(buffer->val, s);
  if (ptr == NULL)
    return -1;
  int index = ptr - buffer->val;
  return index < buffer->n ? index : -1;
}

static void qsplit(qvec l, const char *str, const char *s) {
  char *ptr;
  int len = strlen(s);
  while ((ptr = strstr(str, s)) != NULL) {
    qstr *objs = STR2.create(str, ptr - str);
    Arr.append(l, objs);
    str = ptr + len;
  }
  Arr.append(l, (arr_type) STR2.create(str, strlen(str)));
}
struct QStrUtils StrUtils = {.add=qbufAdd,.sub=qsub,.split=qsplit, .index=qindex};