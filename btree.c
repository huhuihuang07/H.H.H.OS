#include "btree.h"

void BTree_Init(BTreeRoot* root)
{
    root->parent = root->lChild = root->rChild = nullptr;
}

bool BTree_Insert(BTreeRoot* root, BTreeNode_t* node, BTreePos_t pos)
{
    BTreeNode_t* object = BTree_Find(root, node->parent);

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

BTreeNode_t* BTree_Remove(BTreeRoot* root, BTreeNode_t* node)
{
    if (IsEqual(root, nullptr))
    {
        return nullptr;
    }

    BTreeNode_t* object = BTree_Find(root, node);

    if (!IsEqual(object, nullptr))
    {
        BTreeNode_t* parent = object->parent;

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

BTreeNode_t* BTree_Find(BTreeRoot* root, BTreeNode_t* node)
{
    BTreeNode_t* object = nullptr;

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

uint32_t BTree_Degree(BTreeRoot* root)
{
    if (IsEqual(root, nullptr))
    {
        return 0u;
    }

    uint32_t ret = (!!root->lChild) + (!!root->rChild);

    if (IsEqual(ret, 2u))
    {
        return ret;
    }

    if (!IsEqual(root->lChild, nullptr))
    {
        uint32_t lDegree = BTree_Degree(root->lChild);

        ret = Max(ret, lDegree);

        if (IsEqual(ret, 2u))
        {
            return ret;
        }
    }

    if (!(IsEqual(root->rChild, nullptr)))
    {
        uint32_t rDegree = BTree_Degree(root->rChild);

        ret = Max(ret, rDegree);

        if (IsEqual(ret, 2u))
        {
            return ret;
        }
    }

    return ret;
}

uint32_t BTree_Count(BTreeRoot* root)
{
    if (IsEqual(root, nullptr))
    {
        return 0u;
    }

    uint32_t lCount = IsEqual(root->lChild, nullptr) ? 0u : BTree_Count(root->lChild);

    uint32_t rCount = IsEqual(root->rChild, nullptr) ? 0u : BTree_Count(root->rChild);

    return (lCount + rCount + 1u);
}

uint32_t BTree_Hight(BTreeRoot* root)
{
    if (IsEqual(root, nullptr))
    {
        return 0u;
    }

    uint32_t lHight = BTree_Hight(root->lChild);
    uint32_t rHight = BTree_Hight(root->rChild);

    return Max(lHight, rHight) + 1u;
}

void BTree_Clear(BTreeRoot* root, pDestroyFunc_t func)
{
    if (IsEqual(root, nullptr))
    {
        return;
    }

    if (!IsEqual(root->lChild, nullptr))
    {
        BTreeNode_t* child = root->lChild;

        root->lChild = nullptr;

        BTree_Clear(child, func);
    }

    if (!IsEqual(root->rChild, nullptr))
    {
        BTreeNode_t* child = root->rChild;

        root->rChild = nullptr;

        BTree_Clear(child, func);
    }

    func(root);
}