/*
 * properties.c
 *
 *  Created on: Jul 7, 2019
 *  Author: WanQing<1109162935@qq.com>
 */
#include "qlibc/prop.h"
#include "utils/io.h"
#include "qlibc/strutils.h"
#include "std/string.h"
#include "qlibc/control.h"
qprop *prop_create(char *path, char *sep) {
	qreader *reader = reader_create(path);
	RBTree *t = RB.create(typeString, typeString);
	qprop *prop = qmalloc(qprop);
	prop->props = t;
	prop->sep = STR.get(sep);
	prop->src = path;
	if (reader->file) {
		char *ptr;
		while (read_line(reader)) {
			ptr = reader->content;
			char *ptr2 = strchr(ptr, '#');
			if (ptr2)
				*ptr2 = '\0';
			if (*ptr == '\0')
				continue;
			ptr2 = strstr(ptr, sep);
			skyc_assert_(ptr2);
			*ptr2 = '\0';
			ptr2+=prop->sep->len;
			qstr *key = STR.get(str_trim(ptr));
			qstr *val = STR.get(str_trim(ptr2));
			RBNode *node;
			if (RB.insert(t, key, val, &node) == false) {
				node->val = val;
			}
		}
	}
	reader_destroy(reader);
	return prop;
}

void prop_insert(qprop *prop, qstr *key, qstr* val) {
	RBNode *node;
	if (RB.insert(prop->props, key, val, &node) == false) {
		node->val = val;
	}
}

qstr *prop_get(qprop *prop, qstr *key, qstr* defval) {
	RBNode *node = RB.search(prop->props, key);
	if (node)
		return node->val;
	else
		return defval;
}
void prop_destroy(qprop *prop) {
	RB.destroy(&prop->props, NULL);
	qfree(prop);
}
void prop_write(qprop *prop) {
	FILE *fp = fopen(prop->src, "w");
	qstr *sep = prop->sep;
	RBNode *node = RB.min(prop->props);
	qstrbuf buf = { 0 };
	while (node) {
		buf.n = 0;
		qstr *key = node->key;
		qstr *val = node->val;
		STR.add(&buf, key->val, key->len);
		STR.add(&buf, sep->val, sep->len);
		STR.add(&buf, val->val, val->len);
		STR.add(&buf, '\n', 0);
		fwrite(buf.val, buf.n, 1, fp);
		node = RB.next(node);
	}
	fclose(fp);
	if (buf.size)
		qfree(buf.val);
}
const struct apiProp Prop = { prop_create, prop_write, prop_get, prop_insert,
		prop_destroy };
