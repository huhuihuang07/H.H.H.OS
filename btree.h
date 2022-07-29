#ifndef BTREE_H
#define BTREE_H

#include <utility.h>

typedef enum 
{
	BTreePos_ANY,
	BTreePos_LEFT,
	BTreePos_RIGHT,
}BTreePos;

typedef struct _BTreeNode{
	struct _BTreeNode* parent;
	struct _BTreeNode* lChild;
	struct _BTreeNode* rChild;
}BTreeNode;

typedef BTreeNode BTreeRoot;

void BTree_Init(BTreeRoot* root);
bool BTree_Insert(BTreeRoot* root, BTreeNode* node, BTreePos pos);
BTreeNode* BTree_Remove(BTreeRoot* root, BTreeNode* node);
BTreeNode* BTree_Find(BTreeRoot* root, BTreeNode* node);
u32 BTree_Degree(BTreeRoot* root);
u32 BTree_Count(BTreeRoot* root);
u32 BTree_Hight(BTreeRoot* root);
void BTree_Clear(BTreeRoot* root);

#endif //!BTREE_H