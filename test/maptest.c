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
	qentry2 entry2;
	qmap *map;
	mapIter iter;
	CTRL.init();
	qbytes *bytes = Bytes.create(1);
	map = Map.create(_S, typeInt, false);
	for (i = 0; i < TESTSIZE; i++) {
		Map.gset(_S, map, i + 1, true, &entry);
	}
	for (i = 0; i < TESTSIZE; i++) {
		skyc_assert_(Map.gset(_S, map, i + 1, false, &entry) == true);
	}
	typeMap->serialize(bytes, map);
	Map.destroy(map, NULL);
	typeMap->deserial(bytes->data, &map);
	bytes->length = 0;
	Map.iterator(map, &iter);
	while (Map.next(&iter)) {
		qentry2 *entry = iter.entry._2;
		printf("%d\n", entry->key);
	}

	for (i = 0; i < TESTSIZE; i++) {
		skyc_assert_(Map.del(_S, map, i + 1, &entry2));
		skyc_assert_(entry2.key == i + 1);
	}
	skyc_assert_(map->length == 0);

	Map.destroy(map, NULL);

	map = Map.create(_S, typeInt, true);
	map->valtype = typeInt;
	for (i = 0; i < TESTSIZE; i++) {
		Map.gset(_S, map, i + 1, true, &entry);
		entry._2->val = i;
	}
	Map.iterator(map, &iter);
	while (Map.next(&iter)) {
		qentry2 *entry = iter.entry._2;
		printf("%d:%d\n", entry->key, entry->val);
	}
	typeMap->serialize(bytes, map);
	Map.destroy(map, NULL);
	typeMap->deserial(bytes->data, &map);
	for (i = 0; i < TESTSIZE; i++) {
		Map.del(_S, map, i + 1, &entry2);
		skyc_assert_(entry2.val == i);
	}
	skyc_assert_(map->length == 0);
	Map.destroy(map, NULL);
	Bytes.destroy(&bytes);
	CTRL.destroy();
//	Mem.printMeminfo();
	return 0;
}
