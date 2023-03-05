#include "btree.h"

void BTree_Init(BTreeRoot* root)
{
    root->parent = root->lChild = root->rChild = nullptr;
}

bool BTree_Insert(BTreeRoot* root, BTreeNode* node, BTreePos pos)
{
    BTreeNode* object = BTree_Find(root, node->parent);

    bool ret = (!IsEqual(object, nullptr)) && (IsEqual(BTree_Find(root, node), nullptr));

    if (ret)
    {
        switch (pos)
        {
            case BTreePos_ANY: {
                if (ret = IsEqual(object->lChild, nullptr))
                {
                    object->lChild = node;
                }
                else if (ret = IsEqual(object->rChild, nullptr))
                {
                    object->rChild = node;
                }
                break;
            }
            case BTreePos_LEFT: {
                if (ret = IsEqual(object->lChild, nullptr))
                {
                    object->lChild = node;
                }
                break;
            }
            case BTreePos_RIGHT: {
                if (ret = IsEqual(object->rChild, nullptr))
                {
                    object->rChild = node;
                }
                break;
            }
            default: {
                ret = false;
                break;
            }
        }
    }

    return ret;
}

BTreeNode* BTree_Remove(BTreeRoot* root, BTreeNode* node)
{
    if (IsEqual(root, nullptr))
    {
        return nullptr;
    }

    BTreeNode* object = BTree_Find(root, node);

    if (!IsEqual(object, nullptr))
    {
        BTreeNode* parent = object->parent;

        object->parent = nullptr;

        if (IsEqual(parent->lChild, object))
        {
            parent->lChild = nullptr;
        }
        else
        {
            parent->rChild = nullptr;
        }
    }

    return object;
}

BTreeNode* BTree_Find(BTreeRoot* root, BTreeNode* node)
{
    BTreeNode* object = nullptr;

    if (IsEqual(root, node))
    {
        object = node;
    }

    if (IsEqual(root, nullptr))
    {
        return nullptr;
    }

    if (IsEqual(object, nullptr) && (!IsEqual(root->lChild, nullptr)))
    {
        object = BTree_Find(root->lChild, node);
    }

    if (IsEqual(object, nullptr) && (!IsEqual(root->rChild, nullptr)))
    {
        object = BTree_Find(root->rChild, node);
    }

    return object;
}

u32 BTree_Degree(BTreeRoot* root)
{
    if (IsEqual(root, nullptr))
    {
        return 0;
    }

    u32 ret = (!!root->lChild) + (!!root->rChild);

    if (IsEqual(ret, 2))
    {
        return ret;
    }

    if (!IsEqual(root->lChild, nullptr))
    {
        u32 lDegree = BTree_Degree(root->lChild);

        ret = Max(ret, lDegree);

        if (IsEqual(ret, 2))
        {
            return ret;
        }
    }

    if (!(IsEqual(root->rChild, nullptr)))
    {
        u32 rDegree = BTree_Degree(root->rChild);

        ret = Max(ret, rDegree);

        if (IsEqual(ret, 2))
        {
            return ret;
        }
    }

    return ret;
}

u32 BTree_Count(BTreeRoot* root)
{
    if (IsEqual(root, nullptr))
    {
        return 0;
    }

    u32 lCount = IsEqual(root->lChild, nullptr) ? 0 : BTree_Count(root->lChild);

    u32 rCount = IsEqual(root->rChild, nullptr) ? 0 : BTree_Count(root->rChild);

    return (lCount + rCount + 1);
}

u32 BTree_Hight(BTreeRoot* root)
{
    if (IsEqual(root, nullptr))
    {
        return 0;
    }

    u32 lHight = BTree_Hight(root->lChild);
    u32 rHight = BTree_Hight(root->rChild);

    return Max(lHight, rHight) + 1;
}

void BTree_Clear(BTreeRoot* root, pDestroyFunc func)
{
    if (IsEqual(root, nullptr))
    {
        return;
    }

    if (!IsEqual(root->lChild, nullptr))
    {
        BTreeNode* child = root->lChild;

        root->lChild = nullptr;

        BTree_Clear(child, func);
    }

    if (!IsEqual(root->rChild, nullptr))
    {
        BTreeNode* child = root->rChild;

        root->rChild = nullptr;

        BTree_Clear(child, func);
    }

    func(root);
}