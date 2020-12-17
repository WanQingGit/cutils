/*
 * rbtreeTest.c
 *
 *  Created on: Apr 11, 2019
 *      Author: WanQing
 */

#include <stdio.h>
#include "cutils/stl/rbtree.h"
#include <sys/time.h>
#define CHECK_INSERT 0    // "插入"动作的检测开关(0，关闭；1，打开)
#define LENGTH(a) ( (sizeof(a)) / (sizeof(a[0])) )
#define countTime(stat) do{struct timeval start,end; \
gettimeofday(&start, NULL );  \
	stat;\
  gettimeofday(&end, NULL );  \
  double timeuse = ( end.tv_sec - start.tv_sec ) + (end.tv_usec - start.tv_usec)/1000000.0;\
  printf("\ntime=%f\n",timeuse); }while(0)
void printNode(RBNode *node){
	printf("%ld ", node->key);
}
void baseTest() {
	int a[] = { 0,12, 1, 9, 2, 0, 11, 7, 19, 15, 18, /*5,*/
	14, 13, //
			10, 16, 3, 8, 17 };
	int i, ilen = LENGTH(a);
	RBTree* tree;
	printf("baseTest...\n");
	tree = RB.create(NULL,NULL);
	printf("== 原始数据: ");
	for (i = 0; i < ilen; i++)
		printf("%d ", a[i]);
	printf("\n");
	for (i = 0; i < ilen; i++) {
		RB.insert(tree, a[i], NULL,NULL);
	}
	printf("== 前序遍历: ");
	RB.preorder(tree);
	printf("\n== 中序遍历1: ");
	RB.inorder(tree,printNode);
	printf("\n== 中序遍历2: ");
	RBIter* it = RB.getIter(tree);
	while (RB.iterNext(it)) {
		printf("%ld ", it->cur->key);
	}
	RB.releaseIter(it);
	printf("\n== 中序遍历3: ");
	RBNode* node = RB.min(tree);
	while (node != NULL) {
		printf("%ld ", node->key);
		node = RB.next(node);
	}
	printf("\n== 后序遍历: ");
	RB.postorder(tree);
	printf("\n");
	printf("== 最小值: %ld\n", RB.min(tree)->key);
	printf("== 最大值: %ld\n", RB.max(tree)->key);
	i = 4;
	printf("== 离%d最近的值: %ld\n", i, RB.nearest(tree, i, RB_NEAR)->key);
	printf("== 大于%d最小的值: %ld\n", i, RB.nearest(tree, i, RB_ABOVE)->key);
	printf("== 小于%d最大的值: %ld\n", i, RB.nearest(tree, i, RB_BELOW)->key);
	//	printf("== 树的详细信息: \n");
	//	print_rbtree(tree);
	for (i = 0; i < ilen - 4; i++) {
		RB.delete(tree, a[i]);
		//		printf("== 删除节点: %d\n", a[i]);
	}
	printf("\n== 中序遍历: ");
	RB.inorder(tree,printNode);
	printf("\n");
	RB.destroy(&tree, NULL);
}
void performanceTest(int numtest) {
	int i, ilen = 50000;
	RBTree* tree;
	int ntime = 0;
	for (; ntime < numtest; ntime++) {
		printf("performanceTest %d...\n", ntime);
		tree = RB.create(NULL,NULL);
		printf("== 插入数据 ");
		countTime(
		for (i = 0; i < ilen; i++) {
			RB.insert(tree, i, NULL,NULL);
		});
		printf("== 中序遍历1: ");
		countTime(RB.inorder(tree,NULL));
		printf("== 中序遍历2: ");
		countTime(
				RBIter* it = RB.getIter(tree);
		while (RB.iterNext(it)) {
			/*printf("%ld ", it->cur->key);*/ }
		RB.releaseIter(it););
		printf("== 中序遍历3: ");
		countTime(
			RBNode* node = RB.min(tree);
			while (node != NULL) {
//				printf("%ld ", node->key);
				node = RB.next(node); });
		printf("\n");
		printf("== 最小值: %ld\n", RB.min(tree)->key);
		printf("== 最大值: %ld\n", RB.max(tree)->key);
		i = 4;
		printf("== 离%d最近的值: %ld\n", i, RB.nearest(tree, i, RB_NEAR)->key);
		printf("== 大于%d最小的值: %ld\n", i, RB.nearest(tree, i, RB_ABOVE)->key);
		printf("== 小于%d最大的值: %ld\n", i, RB.nearest(tree, i, RB_BELOW)->key);
		printf("== 删除数据");
		countTime(for (i = 0; i < ilen - 4; i++) {
			RB.delete(tree, i);
		});
		printf("\n== 中序遍历: ");
		RB.inorder(tree,printNode);
		printf("\n");
		RB.destroy(&tree, NULL);
	}
}

int main() {
	baseTest();
	performanceTest(3);
	return 0;
}
