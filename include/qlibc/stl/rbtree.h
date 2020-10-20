/*
 * rbtree.h
 *
 *  Created on: Apr 11, 2019
 *      Author: WanQing
 */

#ifndef _RED_BLACK_TREE_H_
#define _RED_BLACK_TREE_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "qlibc/object.h"

typedef intptr_t rbtype;
typedef struct rbData {
	rbtype key;                    // 关键字(键值)
	rbtype val;
} rbData;
// @formatter:off
#define rb_each(tree,stat) do{\
		RBNode *_iter = RB.min(tree);\
		while(_iter){\
			stat\
			_iter = RB.next(_iter);\
		}}while(0)
// @formatter:on
//typedef void* rbtype;
// 红黑树的节点
#define RB_NOT_FREE 0
#define RB_FREE_KEY 1
#define RB_FREE_VAL 1<<1
#define RB_FREE_FORCE 1<<2
#define RB_FREE_BOTH (RB_FREE_KEY|RB_FREE_VAL)
#define RB_FREE_MAX (RB_FREE_BOTH|RB_FREE_FORCE)
typedef struct RBIter {
	RBTree *t;
	RBNode *node;
	RBNode *cur;
	RBNode **stack;
	int top;
} RBIter;

struct apiRB {
	// 创建红黑树，返回"红黑树"！
	RBTree* (*create)(Type typeKey, Type typeVal);
	RBTree* (*clone)(RBTree *tree);
	// 销毁红黑树
	void (*destroy)(RBTree **tree_ptr, void (*destructor)(rbtype, rbtype));
	void (*clear)(RBTree *tree, void (*destructor)(rbtype, rbtype));

	// 将结点插入到红黑树中。插入成功，返回true；失败返回false。
	bool (*insert)(RBTree *tree, rbtype key, rbtype val, RBNode** ptr);
	void (*insertAll)(RBTree *tree, const RBTree *t, rbtype value,
			void (*failfunc)(RBNode *old, RBNode *newNode));

	// 删除结点(key为节点的值)
	void (*delete)(RBTree *tree, rbtype key);

	void (*delNode)(RBTree *tree, RBNode *key);

	// 前序遍历"红黑树"
	void (*preorder)(RBTree *tree);
	// 中序遍历"红黑树"
	void (*inorder)(RBTree *tree, void (*func)(RBNode*));
	// 后序遍历"红黑树"
	void (*postorder)(RBTree *tree);

	// 查找"红黑树"中键值为key的节点。找到的话，返回节点；否则，返回空。
	RBNode* (*search)(RBTree *tree, rbtype key);
	RBIter* (*getIter)(RBTree *tree);
	void (*releaseIter)(RBIter *it);
	RBNode* (*iterNext)(RBIter *it);
	// 返回最小结点的值(将值保存到val中)。找到的话，返回节点；否则，返回空。
	RBNode* (*min)(RBTree *tree);

	// 返回最大结点的值(将值保存到val中)。找到的话，返回节点；否则，返回空。
	RBNode* (*max)(RBTree *tree);
	RBNode* (*nearest)(RBTree *tree, rbtype key, rb_flag flag);
	RBNode* (*next)(RBNode *x);
	RBNode* (*prev)(RBNode *x);
};
extern struct apiRB RB;
#ifdef __cplusplus
}
#endif
#endif
