/*
 * qstrbuffer.c
 *
 *  Created on: Apr 11, 2019
 *  Author: WanQing<1109162935@qq.com>
 */

#include <std/string.h>
#include <stl/vector.h>
#include <string.h>
#include "std/string.h"
#include "strutils.h"
#include "mem.h"

static qstr *string_createlstr(State *L, size_t l, const char *str);

static qstr *string_get(const char *str);

static qstr *newstr(State *S, size_t l, int tag, unsigned int h,
                    const char *str);

static qstr *string_new(const char *str, size_t l);

static void qbufAdd(qstrbuf *buffer, const char *s, int n) {
    if (n > 0) {
        if (buffer->size - buffer->n < n) {
            uint nsize = (buffer->size + n) * 1.9;
            qassert(nsize > buffer->size);
            buffer->val = (char *) realloc(buffer->val, (nsize + 1) * sizeof(char));
            buffer->size = nsize;
        }
        memcpy(buffer->val + buffer->n, s, n * sizeof(char));
        buffer->n += n;
        buffer->val[buffer->n] = '\0';
    } else if (n == 0) {
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
        qstr *objs = string_new(str, ptr - str);
        Arr.append(l, objs);
        str = ptr + len;
    }
    Arr.append(l, (arr_type) string_new(str, strlen(str)));
}

void string_table_resize(State *S, int newsize) {
    int i;
    qstr *p, *hnext;
    stringtable *tb = &(S->g->strt);
    if (newsize != tb->size) { /* grow table if needed */
        mem_realloc_vector(S, tb->ht, tb->size, newsize, qstr*);
    }
    int mod = newsize - 1;
    for (i = 0; i < tb->size; i++) { /* rehash */
        p = tb->ht[i];
        tb->ht[i] = NULL;
        while (p) { /* for each node in the list */
            hnext = p->hnext; /* save next */
            unsigned int h = cast(uint, p->hash & mod); /* new position */
            p->hnext = tb->ht[h]; /* chain it */
            tb->ht[h] = p;
            p = hnext;
        }
    }
    tb->size = newsize;
}

static qstr *gshrstr(State *S, const char *str, size_t l) {
    qstr *ts;
    gl_state *g = S->g;
    unsigned int h = str_hash_count(str, l, g->seed);
    qstr **list = &g->strt.ht[h & (g->strt.size - 1)];
    qassert(str != NULL);
    for (ts = *list; ts; ts = ts->hnext) {
        if (l == ts->len && h == ts->hash
            && (memcmp(str, gstr(ts), l * sizeof(char)) == 0)) {
//            if (isdead(g, o2gc(ts))) /* dead (but not collected yet)? */
//                changewhite(o2gc(ts)); /* resurrect it */
            return ts;
        }
    }
    if (g->strt.nuse >= g->strt.size && g->strt.size <= UINT_MAX / 2) {
        string_table_resize(S, g->strt.size * 2);
        list = &g->strt.ht[h & (g->strt.size - 1)]; /* recompute with new size */
    }
    ts = newstr(S, l, V_SHRSTR, h, str);
    ts->hnext = *list;
    *list = ts;
    g->strt.nuse++;
    return ts;
}

static qstr *string_get(const char *str) {
    size_t len=strlen(str);
    uint hash=str_hash_count(str, len, _S->g->seed);
    uint i = hash & (STRCACHE_N - 1); /* hash */
    int j;
    qstr **p = _S->g->strcache[i];
    for (j = 0; j < STRCACHE_M; j++) {
        if (/*p[j]->tt&&*/strcmp(str, p[j]->val) == 0) /* hit? */
            return p[j]; /* that is it */
    }
    /* normal route */
    for (j = STRCACHE_M - 1; j; j--)
        p[j] = p[j - 1]; /* move out last element */
    /* new element is first in the list */
    p[0] = string_new(str, strlen(str));
    return p[0];
}

static qstr *string_new(const char *str, size_t l) {
    if (l <= MAXSHORTLEN)
        return gshrstr(_S, str, l);
    else {
        qstr *ts = string_createlstr(_S, l, str);
        return ts;
    }
}

static qstr *string_createlstr(State *S, size_t l, const char *str) {
    qstr *ts = newstr(S, l, V_LNGSTR, 0, str);
    return ts;
}

static qstr *newstr(State *S, size_t l, int tag, unsigned int h,
                    const char *str) {
    size_t totalsize = sizeof(qstr) + (l + 1) * sizeof(char);
    qstr *ts = cast(qstr*, Mem.new(S, tag, totalsize));
    ts->hash = h;
    ts->len = l;
    memcpy(gstr(ts), str, l * sizeof(char));
    gstr(ts)[l] = '\0';
    return ts;
}

void strt_destroy() {
    stringtable *strt = &_S->g->strt;
    qstr *str;
    int size = strt->size;
    for (int i = 0; i < size; i++) {
        str = strt->ht[i];
        while (str) {
            qstr *s = str;
            str = str->hnext;
            Mem.alloc(_S, o2gc(s), sizeof(qstr) + (s->len + 1) + sizeof(GCObj),
                      0);
        }
        strt->ht[i] = NULL;
    }
    strt->nuse = 0;
}


struct strApi STR = {string_new,string_get, string_table_resize, qbufAdd, qsub,
                     qsplit, qindex};
