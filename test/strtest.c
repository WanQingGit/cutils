/*
 * strtest.c
 *
 *  Created on: Dec 21, 2020
 *      Author: WanQing
 */
#include "cutils/std/string.h"
#include "cutils/control.h"

#include <sys/time.h>
#define countTime(stat) do{struct timeval start,end; \
gettimeofday(&start, NULL );  \
	stat;\
  gettimeofday(&end, NULL );  \
  double timeuse = ( end.tv_sec - start.tv_sec ) + (end.tv_usec - start.tv_usec)/1000000.0;\
  printf("\ntime=%f\n\n",timeuse); }while(0)

void testStrApi(struct QString *STR2);

int main(){
//  malloc(8);
  CTRL.init(NULL);
  STR2.init_env();
  countTime(testStrApi(&STR););
  countTime(testStrApi(&STR2););
  STR2.destroy();
  CTRL.destroy();
	return 0;
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
