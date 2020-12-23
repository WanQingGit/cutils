/*
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

typedef struct qstrbuffer {
    uint size;
    uint n; //used
    char *val;
} qstrbuf;

size_t  str_hash_count(const char *str, size_t l, uint seed);
char *str_trim(char *s);
char* skip_empty(char* s);
char* str_rtrim(char* s);
char* ltrim(char* s);
bool numchar(char* s);
char* strip(char* s);
char* backspace(char* s, char** cursor);
void *qmemcpy(void *dst, const void *src, size_t num);
char skip_space(char** s);

uint makeseed(State *S);

struct QStrUtils{
#define buf_add(buf,s) StrUtils.add(buf,s,strlen(s))
    void (*add)(qstrbuf *buffer, const char *s, int n);
    /*
     * 将字符串s中的p替换成r,例如s="a,b" p="," r="_" 最后结果是"a_b"
     * */
    void (*sub)(qstrbuf *buffer, const char *s, const char *p, const char *r);

    void (*split)(qvec l, const char *str, const char *s);

    int (*index)(qstrbuf *buffer, const char *s);
};
extern struct QStrUtils StrUtils;
#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_QSTRING_H_ */
