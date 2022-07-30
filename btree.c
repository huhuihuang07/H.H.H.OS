#include <btree.h>

void BTree_Init(BTreeRoot* root)
{
	root->parent = root->lChild = root->rChild = nullptr;
}

bool BTree_Insert(BTreeRoot* root, BTreeNode* node, BTreePos pos)
{
	BTreeNode* object = BTree_Find(root, node->parent);

	bool ret = (!IsEqual(object, nullptr)) && (IsEqual(BTree_Find(root, node), nullptr));

	if(ret){
		switch(pos)
		{
			case BTreePos_ANY : 
			{
				if(IsEqual(object->lChild, nullptr)){
					object->lChild = node;
				}else if(IsEqual(object->rChild, nullptr)){
					object->rChild = node;
				}else{
					ret = false;
				}
				break;
			}
			case BTreePos_LEFT :
			{
				if(IsEqual(object->lChild, nullptr)){
					object->lChild = node;
				}else{
					ret = false;
				}
				break;
			}
			case BTreePos_RIGHT :
			{
				if(IsEqual(object->rChild, nullptr)){
					object->rChild = node;
				}else{
					ret = false;
				}
				break;
			}
			default : ret = false;
		}
	}

	return ret;
}

BTreeNode* BTree_Remove(BTreeRoot* root, BTreeNode* node)
{
	BTreeNode* object = BTree_Find(root, node);

	if(!IsEqual(object, nullptr)){
		BTreeNode* parent = object->parent;

		object->parent = nullptr;

		if(IsEqual(parent->lChild, object)){
			parent->lChild = nullptr;
		}else{
			parent->rChild = nullptr;
		}
	}

	return object;
}

BTreeNode* BTree_Find(BTreeRoot* root, BTreeNode* node)
{
	BTreeNode* object = nullptr;

	if(IsEqual(root, node)){
		object = node;
	}

	if(IsEqual(object, nullptr) && (!IsEqual(root->lChild, nullptr))){
		object = BTree_Find(root->lChild, node);
	}

	if(IsEqual(object, nullptr) && (!IsEqual(root->rChild, nullptr))){
		object = BTree_Find(root->rChild, node);
	}

	return object;
}

u32 BTree_Degree(BTreeRoot* root)
{
	u32 ret = (!!root->lChild) + (!!root->rChild);

	if(IsEqual(ret, 2)){
		return ret;
	}

	if(!IsEqual(root->lChild, nullptr))
	{
		u32 lDegree = BTree_Degree(root->lChild);

		ret = Max(ret, lDegree);

		if(IsEqual(ret, 2)){
			return ret;
		}
	}

	if(!(IsEqual(root->rChild, nullptr)))
	{
		u32 rDegree = BTree_Degree(root->rChild);

		ret = Max(ret, rDegree);

		if(IsEqual(ret, 2)){
			return ret;
		}
	}

	return ret;
}

u32 BTree_Count(BTreeRoot* root)
{
	u32 lCount = IsEqual(root->lChild, nullptr) ? 0 : BTree_Count(root->lChild);

	u32 rCount = IsEqual(root->rChild, nullptr) ? 0 : BTree_Count(root->rChild);

	return (lCount + rCount + 1);
}

u32 BTree_Hight(BTreeRoot* root)
{
	if(IsEqual(root, nullptr)){
		return 0;
	}

	u32 lHight = BTree_Hight(root->lChild);
	u32 rHight = BTree_Hight(root->rChild);

	return Max(lHight, rHight) + 1;
}

void BTree_Clear(BTreeRoot* root)
{
	// TODO
}