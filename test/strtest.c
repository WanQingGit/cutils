/*
 * strtest.c
 *
 *  Created on: Dec 21, 2020
 *      Author: WanQing
 */
#include "cutils/std/string.h"
#include "cutils/control.h"
#include "cutils/stl/map.h"
#include <sys/time.h>
#include <cutils/strutils.h>

#define countTime(stat) do{struct timeval start,end; \
gettimeofday(&start, NULL );  \
	stat;\
  gettimeofday(&end, NULL );  \
  double timeuse = ( end.tv_sec - start.tv_sec ) + (end.tv_usec - start.tv_usec)/1000000.0;\
  printf("\ntime=%f\n\n",timeuse); }while(0)

void testStrApi(struct QString *STR2);
void testStrUtils();
int main(){
  CTRL.init(NULL);
  STR2.init_env();
  testStrUtils();
  countTime(testStrApi(&STR););
  countTime(testStrApi(&STR2););
  STR2.destroy();
  CTRL.destroy();
	return 0;
}
static qmap *map;
void init_type_table(){
  map=Map.create(typeString,MAP_TABLE);
  qentry entry;
  assert(!Map.gset(map,STR2.get("boolean"),true,&entry));
  map_set_val(entry,"Z");
  assert(!Map.gset(map,STR2.get("byte"),true,&entry));
  map_set_val(entry,"B");
  assert(!Map.gset(map,STR2.get("char"),true,&entry));
  map_set_val(entry,"C");
  assert(!Map.gset(map,STR2.get("short"),true,&entry));
  map_set_val(entry,"S");
  assert(!Map.gset(map,STR2.get("int"),true,&entry));
  map_set_val(entry,"I");
  assert(!Map.gset(map,STR2.get("long"),true,&entry));
  map_set_val(entry,"J");
  assert(!Map.gset(map,STR2.get("float"),true,&entry));
  map_set_val(entry,"F");
  assert(!Map.gset(map,STR2.get("double"),true,&entry));
  map_set_val(entry,"D");
  assert(!Map.gset(map,STR2.get("void"),true,&entry));
  map_set_val(entry,"V");
}
void addType(qstrbuf *buf,qstr *argtype){
  qentry entry;
  if(argtype->val[0]=='['){
    buf_add(buf,str_val(argtype));
//    STR.sub(buf,str_val(argtype),".","/");
  } else if(Map.gset(map,argtype,false,&entry)){
    buf_add(buf,(char*)entry.dict->val);
  }else{
    StrUtils.add(buf,(char*)'L',-1);
    buf_add(buf,str_val(argtype));
    StrUtils.add(buf,(char*)';',-1);
  }
}
void testStrUtils(){
  init_type_table();
  qstrbuf buf={0},buf2={0};
  buf_add(&buf,"(");
  addType(&buf,STR2.get("java.lang.String"));
  addType(&buf,STR2.get("int"));
  buf_add(&buf,")");
  addType(&buf,STR2.get("void"));
  StrUtils.sub(&buf2,buf.val,".","/");
  printf("%s\n",buf2.val);
  free(buf.val);
  free(buf2.val);
}
void testStrApi(struct QString *STR2) {
  size_t presize=STR2->size();
  qstr *str1=STR2->get("nihao");
  qstr *str2=STR2->get("nihao");
  assert(str1 == str2);
  char buf[48];
  for(int z=0;z<10;z++){
    for(int i=0;i<10240;i++){
      sprintf(buf,"test_str:%d",i);
      str1=STR2->get(buf);
      assert(strcmp(buf,str1->val)==0);
      str2=STR2->get(buf);
      assert(str1==str2);
    }
  }
  size_t currentSize=STR2->size();
  printf("pre:%ld current:%ld diff:%ld\n",presize,currentSize,currentSize-presize);
}
