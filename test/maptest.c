/*
 * maptest.c
 *
 *  Created on: Jul 4, 2019
 *  Author: WanQing<1109162935@qq.com>
 */

#include "cutils/stl/map.h"
#include "cutils/control.h"
#include "cutils/mem.h"
#include "cutils/stl/bytelist.h"
#define TESTSIZE 100
int main() {
	int i;
	qentry entry;
	qentry_dict entry2;
	qmap *map;
	mapIter iter;
	CTRL.init(NULL);
	qbytes *bytes = Bytes.create(1);
	map = Map.create(typeInt, false);
	for (i = 0; i < TESTSIZE; i++) {
		Map.gset( map, i + 1, true, &entry);
	}
	for (i = 0; i < TESTSIZE; i++) {
		qassert(Map.gset(map, i + 1, false, &entry) == true);
	}
	typeMap->serialize(bytes, map);
	Map.destroy(map);
	typeMap->deserial(bytes->data, &map);
	bytes->length = 0;
	Map.iterator(map, &iter);
	while (Map.next(&iter)) {
		qentry_dict *entry = iter.entry.dict;
		printf("%d\n", entry->key);
	}

	for (i = 0; i < TESTSIZE; i++) {
    qassert(Map.del(map, i + 1, &entry2));
    qassert(entry2.key == i + 1);
	}
  qassert(map->length == 0);

	Map.destroy(map);

	map = Map.create( typeInt, true);
	map->valtype = typeInt;
	for (i = 0; i < TESTSIZE; i++) {
		Map.gset( map, i + 1, true, &entry);
		entry.dict->val = i;
	}
	Map.iterator(map, &iter);
	while (Map.next(&iter)) {
		qentry_dict *entry = iter.entry.dict;
		printf("%d:%d\n", entry->key, entry->val);
	}
	typeMap->serialize(bytes, map);
	Map.destroy(map);
	typeMap->deserial(bytes->data, &map);
	for (i = 0; i < TESTSIZE; i++) {
		Map.del( map, i + 1, &entry2);
    qassert(entry2.val == i);
	}
  qassert(map->length == 0);
	Map.destroy(map);
	Bytes.destroy(&bytes);
	CTRL.destroy();
	return 0;
}
