/*
 * maptest.c
 *
 *  Created on: Jul 4, 2019
 *  Author: WanQing<1109162935@qq.com>
 */

#include <cutils/stl/vector.h>
#include "cutils/stl/map.h"
#include "cutils/std/string.h"
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
  STR2.init_env();
	map=Map.create(&STR2.type,MAP_TABLE|MAP_FREE_VAL);
	map->valtype=typeList;
	qentry qentry1;
	assert(!Map.gset(map,STR2.get("nihao"),true,&qentry1));
	qvec vec=Arr.create(typeInt,8,ARR_TYPE|ARR_PTR);
	qentry1.dict->val=vec;
	Arr.append(vec,12);
	Arr.append(vec,13);
	Map.iterator(map,&iter);
  while (Map.next(&iter)) {
    qstr *qstr1 = (qstr *) iter.entry.dict->key;
    printf("key:%s\n",qstr1->val);
    vec = (qvec) iter.entry.dict->val;
    for(int i=0;i<vec->length;i++){
      printf("%d\n",Arr.at(vec,i));
    }
    Arr.destroy(&vec);
    assert(Map.del(map,iter.entry.dict->key,NULL));
  }
  Map.destroy(map);

	qbytes *bytes = Bytes.create(1);
	map = Map.create(typeInt, MAP_SET);
	for (i = 0; i < TESTSIZE; i++) {
		Map.gset( map, i + 1, true, &entry);//值类型无需再赋值
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
	STR2.destroy();
	CTRL.destroy();
	return 0;
}
