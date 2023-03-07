#pragma once

#include "utility.h"

typedef enum
{
	BTreePos_ANY = 1 << 0,
	BTreePos_LEFT = 1 << 1,
	BTreePos_RIGHT = BTreePos_LEFT | BTreePos_ANY,
} BTreePos_t;

typedef struct _BTreeNode_t
{
	struct _BTreeNode_t *parent;
	struct _BTreeNode_t *lChild;
	struct _BTreeNode_t *rChild;
} BTreeNode_t;

typedef BTreeNode_t BTreeRoot;

typedef void (*pDestroyFunc_t)(BTreeNode_t *);

void BTree_Init(BTreeRoot *root);
bool BTree_Insert(BTreeRoot *root, BTreeNode_t *node, BTreePos_t pos);
BTreeNode_t *BTree_Remove(BTreeRoot *root, BTreeNode_t *node);
BTreeNode_t *BTree_Find(BTreeRoot *root, BTreeNode_t *node);
uint32_t BTree_Degree(BTreeRoot *root);
uint32_t BTree_Count(BTreeRoot *root);
uint32_t BTree_Hight(BTreeRoot *root);
void BTree_Clear(BTreeRoot *root, pDestroyFunc_t func);