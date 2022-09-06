#ifndef BTREE_H
#define BTREE_H

#include "utility.h"

typedef enum 
{
	BTreePos_ANY = 1 << 0,
	BTreePos_LEFT = 1 << 1,
	BTreePos_RIGHT = BTreePos_LEFT | BTreePos_ANY,
}BTreePos;

typedef struct _BTreeNode{
	struct _BTreeNode* parent;
	struct _BTreeNode* lChild;
	struct _BTreeNode* rChild;
}BTreeNode;

typedef BTreeNode BTreeRoot;

typedef void (* pDestroyFunc)(BTreeNode*);

void BTree_Init(BTreeRoot* root);
bool BTree_Insert(BTreeRoot* root, BTreeNode* node, BTreePos pos);
BTreeNode* BTree_Remove(BTreeRoot* root, BTreeNode* node);
BTreeNode* BTree_Find(BTreeRoot* root, BTreeNode* node);
u32 BTree_Degree(BTreeRoot* root);
u32 BTree_Count(BTreeRoot* root);
u32 BTree_Hight(BTreeRoot* root);
void BTree_Clear(BTreeRoot* root, pDestroyFunc func);

#endif //!BTREE_H