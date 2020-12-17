/*
 * StrUtil.h
 *
 *  Created on: Sep 11, 2018
 *      Author: WanQing
 */

#ifndef INCLUDE_QSTRING_H_
#define INCLUDE_QSTRING_H_
#ifdef __cplusplus
extern "C"{
#endif
#include"cutils/object.h"
#include "cutils/utils/math.h"
typedef enum {
#define BIT_ALPHA	0
	MASK_ALPHA = MASK(BIT_ALPHA),
#define BIT_DIGIT	1
	MASK_DIGIT = MASK(BIT_DIGIT),
#define BIT_PRINT	2
	MASK_PRINT = MASK(BIT_PRINT),
#define BIT_SPACE	3
	MASK_SPACE = MASK(BIT_SPACE),
#define BIT_XDIGIT	4
	MASK_XDIGIT = MASK(BIT_XDIGIT),
} typechar;
extern const typechar char_type[];
#define is_alp(c)   (char_type[c]&MASK_ALPHA)
#define is_dec(c)   (char_type[c]&MASK_DIGIT)
#define is_print(c)	(char_type[c]&MASK_PRINT)
#define is_space(c) (char_type[c]&MASK_SPACE)
#define is_hex(c)   (char_type[c]&MASK_XDIGIT)


#define gstr(s)   (s->val)

#define gco2s(o) cast(qstr*,o)
#define HASHLIMIT		5
#define is_newline(c) ({char _c=(c);(_c=='\n'||_c=='\r');})
#define EOZ	(-1)
#define nextchar(p) (*++p)
uint str_hash_count(char *str, size_t l, uint seed);
char *str_trim(char *s);
char* skip_empty(char* s);
char* str_rtrim(char* s);
char* ltrim(char* s);
bool numchar(char* s);
char* strip(char* s);
char* backspace(char* s, char** cursor);
void *qmemcpy(void *dst, const void *src, size_t num);
char skip_space(char** s);
//qstr *skys_new(State *L, const char *str, size_t l);

uint makeseed(State *S);
//qstr *string_get(State *S, const char *str);
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QSTRING_H_ */
