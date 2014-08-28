#include "avl_tree.h"

static int _avl_insert(avl_tree_t *tree, avl_node_t *node, int key, bool *taller, void *data);
static void _avl_destroy(avl_node_t *node);

static int avl_right_balance(avl_tree_t *tree, avl_node_t *node);
static int avl_left_balance(avl_tree_t *tree, avl_node_t *node);
static int avl_insert_right(avl_tree_t *tree, avl_node_t *node, int key, bool *taller, void *data);
static int avl_insert_left(avl_tree_t *tree, avl_node_t *node, int key, bool *taller, void *data);

static int _avl_delete(avl_tree_t *tree, avl_node_t *node, int key, bool *lower, void **data);
static int avl_replace_and_delete(avl_tree_t *tree, avl_node_t *node, avl_node_t *replace, bool *lower);
static int avl_delete_left_balance(avl_tree_t *tree, avl_node_t *node, bool *lower);
static int avl_delete_right_balance(avl_tree_t *tree, avl_node_t *node, bool *lower);

/******************************************************************************
 **函数名称: avl_creat
 **功    能: 创建平衡二叉树对象(对外接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **输出参数: NONE
 **返    回: AVL_SUCCESS:成功 AVL_FAILED:失败
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.19 #
 ******************************************************************************/
int avl_creat(avl_tree_t **tree)
{
    *tree = (avl_tree_t *)calloc(1, sizeof(avl_tree_t));
    if (NULL == *tree)
    {
        return AVL_FAILED;
    }

    (*tree)->root = NULL;
    return AVL_SUCCESS;
}

/******************************************************************************
 **函数名称: avl_insert
 **功    能: 插入新结点(对外接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 新结点
 **     data: 附加数据
 **输出参数: NONE
 **返    回: AVL_SUCCESS:成功 AVL_NODE_EXIST:已存在 AVL_FAILED:失败
 **实现描述: 
 **     1. 当树的根结点为空时，则直接创建根结点，并赋值
 **     2. 当树的根结点不为空时，则调用_insert()进行处理
 **注意事项: 
 **     1. 当key已经存在时，并不会重置data值. 
 **     2. 如果要重置data值，请先执行删除操作.
 **作    者: # Qifeng.zou # 2013.12.12 #
 ******************************************************************************/
int avl_insert(avl_tree_t *tree, int key, void *data)
{
    bool taller = false;
    avl_node_t *node = tree->root;

    /* 如果为空树，则创建第一个结点 */
    if (NULL == node)
    {
        node = (avl_node_t *)calloc(1, sizeof(avl_node_t));
        if (NULL == node)
        {
            return AVL_FAILED;
        }
        node->parent = NULL;
        node->rchild = NULL;
        node->lchild = NULL;
        node->bf = AVL_EH;
        node->key = key;
        node->data = data;
        
        tree->root = node;
        return AVL_SUCCESS;
    }

    return _avl_insert(tree, node, key, &taller, data);
}

/******************************************************************************
 **函数名称: _avl_insert
 **功    能: 插入新结点(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 需在该结点的子树上插入key
 **     key: 需被插入的key
 **     data: 附加数据
 **输出参数: 
 **     taller: 是否增高
 **返    回: AVL_SUCCESS:成功 AVL_NODE_EXIST:已存在 AVL_FAILED:失败
 **实现描述: 
 **     1. 当结点关键字等于key值时，结点已存在
 **     2. 当结点关键字小于key值时，插入右子树
 **     3. 当结点关键字大于key值时，插入左子树
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.13 #
 ******************************************************************************/
static int _avl_insert(avl_tree_t *tree, avl_node_t *node, int key, bool *taller, void *data)
{
    if (key == node->key)        /* 结点已存在 */
    {
        *taller = false;
        return AVL_NODE_EXIST;
    }
    else if (key > node->key)    /* 插入右子树 */
    {
        return avl_insert_right(tree, node, key, taller, data);
    }

    /* 插入左子树 */
    return avl_insert_left(tree, node, key, taller, data);
}

/******************************************************************************
 **函数名称: avl_insert_right
 **功    能: 在node的右子树中插入新结点(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 需在该结点的子树上插入key
 **     key: 需被插入的key
 **输出参数: 
 **     taller: 是否增高
 **返    回: AVL_SUCCESS:成功 AVL_NODE_EXIST:已存在 AVL_FAILED:失败
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.13 #
 ******************************************************************************/
static int avl_insert_right(
    avl_tree_t *tree, avl_node_t *node, int key, bool *taller, void *data)
{
    int ret = -1;
    avl_node_t *add = NULL;
    
    if (NULL == node->rchild)
    {
        add = (avl_node_t *)calloc(1, sizeof(avl_node_t));
        if (NULL == add)
        {
            *taller = false;
            fprintf(stderr, "[%s][%d] Calloc failed!\n", __FILE__, __LINE__);
            return AVL_FAILED;
        }

        add->lchild = NULL;
        add->rchild = NULL;
        add->parent = node;
        add->key = key;
        add->bf = AVL_EH;
        add->data = data;
        
        node->rchild = add;
        *taller = true;     /* node的高度增加了 */
    }
    else
    {
        ret = _avl_insert(tree, node->rchild, key, taller, data);
        if ((AVL_SUCCESS != ret))
        {
            return ret;
        }
    }

    if (false == *taller)
    {
        return AVL_SUCCESS;
    }

    /* 右增高: 进行平衡化处理 */
    switch(node->bf)
    {
        case AVL_LH:    /* 左高: 右子树增高 不会导致失衡 */
        {
            node->bf = AVL_EH;
            *taller = false;
            return AVL_SUCCESS;
        }
        case AVL_EH:    /* 等高: 右子树增高 不会导致失衡 */
        {
            node->bf = AVL_RH;
            *taller = true;
            return AVL_SUCCESS;
        }
        case AVL_RH:    /* 右高: 右子树增高 导致失衡 */
        {
            avl_right_balance(tree, node);
            *taller = false;
            return AVL_SUCCESS;
        }
    }

    return AVL_FAILED;
}

/******************************************************************************
 **函数名称: avl_insert_left
 **功    能: 在node的左子树中插入新结点(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 需在该结点的子树上插入key
 **     key: 需被插入的key
 **输出参数: 
 **     taller: 是否增高
 **返    回: AVL_SUCCESS:成功 AVL_NODE_EXIST:已存在 AVL_FAILED:失败
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.13 #
 ******************************************************************************/
static int avl_insert_left(
    avl_tree_t *tree, avl_node_t *node, int key, bool *taller, void *data)
{
    int ret = -1;
    avl_node_t *add = NULL;
    
    if (NULL == node->lchild)
    {
        add = (avl_node_t *)calloc(1, sizeof(avl_node_t));
        if (NULL == add)
        {
            *taller = false;
            fprintf(stderr, "[%s][%d] Calloc failed!\n", __FILE__, __LINE__);
            return AVL_FAILED;
        }

        add->lchild = NULL;
        add->rchild = NULL;
        add->parent = node;
        add->key = key;
        add->bf = AVL_EH;
        add->data = data;
        
        node->lchild = add;
        *taller = true;     /* node的高度增加了 */
    }
    else
    {
        ret = _avl_insert(tree, node->lchild, key, taller, data);
        if (AVL_SUCCESS != ret)
        {
            return ret;
        }
    }

    if (false == *taller)
    {
        return AVL_SUCCESS;
    }

    /* 左增高: 进行平衡化处理 */
    switch(node->bf)
    {
        case AVL_RH:    /* 右高: 左子树增高 不会导致失衡 */
        {
            node->bf = AVL_EH;
            *taller = false;
            return AVL_SUCCESS;
        }
        case AVL_EH:    /* 等高: 左子树增高 不会导致失衡 */
        {
            node->bf = AVL_LH;
            *taller = true;
            return AVL_SUCCESS;
        }
        case AVL_LH:    /* 左高: 左子树增高 导致失衡 */
        {
            avl_left_balance(tree, node);
            *taller = false;
            return AVL_SUCCESS;
        }
    }

    return AVL_FAILED;
}

/******************************************************************************
 **函数名称: avl_rr_balance
 **功    能: RR型平衡化处理 - 向左旋转(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 右边失去平衡的结点
 **输出参数: NONE
 **返    回: AVL_SUCCESS:成功 AVL_FAILED:失败
 **实现描述: RR型
 **              A                  C
 **             / \                / \
 **            AL  C      ->      A  CR
 **               / \            / \   \
 **              CL CR          AL CL   X
 **                   \
 **                    X
 **              (1)                (2)
 ** 说明: 结点A是失衡结点，此时当结点C的平衡因子为-1时，可判断为RR型。
 **注意事项: 
 **     1. 图(1)中A表示右边失衡的结点 图(2)表示平衡处理的结果
 **作    者: # Qifeng.zou # 2013.12.13 #
 ******************************************************************************/
static int avl_rr_balance(avl_tree_t *tree, avl_node_t *node)
{
    avl_node_t *rchild = node->rchild, *parent = node->parent;

    avl_set_rchild(node, rchild->lchild);
    node->bf = AVL_EH;

    avl_set_lchild(rchild, node);
    rchild->bf = AVL_EH;

    avl_replace_child(tree, parent, node, rchild);

    return AVL_SUCCESS;
}

/******************************************************************************
 **函数名称: avl_rl_balance
 **功    能: RL型平衡化处理 - 先向右旋转 再向左旋转(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 右边失去平衡的结点
 **输出参数: NONE
 **返    回: AVL_SUCCESS:成功 AVL_FAILED:失败
 **实现描述: 
 ** 场景1: RL型
 **              A                    B
 **             / \                /    \
 **            AL  C      ->      A      C
 **               / \            / \    / \
 **              B  CR          AL BL  BR CR
 **             / \
 **            BL BR
 **              (1)                (2)
 ** 说明: 结点A是失衡结点，此时当结点C的平衡因子为1时，可判断为RL型。
 **       虽然此时结点B的平衡因子的值可能为:-1, 0, 1. 
 **       但旋转处理的方式是一致的，只是旋转之后的平衡因子不一致.
 **注意事项: 
 **     1. 图(1)中A表示右边失衡的结点 图(2)表示平衡处理的结果
 **作    者: # Qifeng.zou # 2013.12.13 #
 ******************************************************************************/
int avl_rl_balance(avl_tree_t *tree, avl_node_t *node)
{
    avl_node_t *rchild = node->rchild,
        *parent = node->parent, *rlchild = NULL;
    
    rlchild = rchild->lchild;
    switch(rlchild->bf)
    {
        case AVL_LH:
        {
            node->bf = AVL_EH;
            rchild->bf = AVL_RH;
            rlchild->bf = AVL_EH;
            break;
        }
        case AVL_EH:
        {
            node->bf = AVL_EH;
            rchild->bf = AVL_EH;
            rlchild->bf = AVL_EH;
            break;
        }
        case AVL_RH:
        {
            node->bf = AVL_LH;
            rchild->bf = AVL_EH;
            rlchild->bf = AVL_EH;
            break;
        }
    }

    avl_set_lchild(rchild, rlchild->rchild);
    avl_set_rchild(rlchild, rchild); 
    avl_set_rchild(node, rlchild->lchild);
    avl_set_lchild(rlchild, node);

    avl_replace_child(tree, parent, node, rlchild);
    
    return AVL_SUCCESS;
}

/******************************************************************************
 **函数名称: avl_right_balance
 **功    能: 对右边失去平衡的结点进行平衡化处理(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 右边失去平衡的结点
 **输出参数: NONE
 **返    回: AVL_SUCCESS:成功 AVL_FAILED:失败
 **实现描述: 
 ** 场景1: RR型
 **              A                  C
 **             / \                / \
 **            AL  C      ->      A  CR
 **               / \            / \   \
 **              CL CR          AL CL   X
 **                   \
 **                    X
 **              (1)                (2)
 ** 说明: 结点A是失衡结点，此时当结点C的平衡因子为-1时，可判断为RR型。
 ** 场景2: RL型
 **              A                    B
 **             / \                /    \
 **            AL  C      ->      A      C
 **               / \            / \    / \
 **              B  CR          AL BL  BR CR
 **             / \
 **            BL BR
 **              (1)                (2)
 ** 说明: 结点A是失衡结点，此时当结点C的平衡因子为1时，可判断为RL型。
 **       虽然此时结点B的平衡因子的值可能为:-1, 0, 1. 
 **       但旋转处理的方式是一致的，只是旋转之后的平衡因子不一致.
 **注意事项: 
 **     1. 图(1)中A表示右边失衡的结点 图(2)表示平衡处理的结果
 **作    者: # Qifeng.zou # 2013.12.13 #
 ******************************************************************************/
static int avl_right_balance(avl_tree_t *tree, avl_node_t *node)
{
    avl_node_t *rchild = node->rchild;

    switch(rchild->bf)
    {
        case AVL_RH:    /* 场景1: RR型 - 向左旋转 */
        {
            return avl_rr_balance(tree, node);
        }
        case AVL_LH:    /* 场景2: RL型 - 先向右旋转 再向左旋转 */
        {
            return avl_rl_balance(tree, node);
        }
    }
    
    return AVL_FAILED;
}

/******************************************************************************
 **函数名称: avl_ll_balance
 **功    能: LL型平衡化处理 - 向右旋转(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 左边失去平衡的结点
 **输出参数: NONE
 **返    回: AVL_SUCCESS:成功 AVL_FAILED:失败
 **实现描述: 
 ** 场景1: LL型
 **              A                  B
 **             / \                / \
 **            B   C      ->      BL  A
 **           / \                /   / \
 **          BL BR              X   BR  C
 **         /
 **        X
 **             (1)                (2)
 ** 说明: 结点A是失衡结点，此时当结点B的平衡因子为1时，可判断为LL型。
 **注意事项: 
 **     1. 图(1)中A表示左边失衡的结点 图(2)表示平衡处理的结果
 **作    者: # Qifeng.zou # 2013.12.13 #
 ******************************************************************************/
int avl_ll_balance(avl_tree_t *tree, avl_node_t *node)
{
    avl_node_t *lchild = node->lchild, *parent = node->parent;

    avl_set_lchild(node, lchild->rchild);
    node->bf = AVL_EH;

    avl_set_rchild(lchild, node);
    lchild->bf = AVL_EH;

    avl_replace_child(tree, parent, node, lchild);

    return AVL_SUCCESS;
}

/******************************************************************************
 **函数名称: avl_lr_balance
 **功    能: LR型平衡化处理 - 先左旋转 再向右旋转(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 左边失去平衡的结点
 **输出参数: NONE
 **返    回: AVL_SUCCESS:成功 AVL_FAILED:失败
 **实现描述: 
 ** 场景1: LL型
 **              A                  B
 **             / \                / \
 **            B   C      ->      BL  A
 **           / \                /   / \
 **          BL BR              X   BR  C
 **         /
 **        X
 **             (1)                (2)
 ** 说明: 结点A是失衡结点，此时当结点B的平衡因子为1时，可判断为LL型。
 ** 场景2: LR型
 **              A                    C
 **             / \                /     \
 **            B  AR      ->      B       A
 **           / \                / \     / \
 **          BL  C              BL CL   CR AR
 **             / \
 **            CL CR
 **             (1)                (2)
 ** 说明: 结点A是失衡结点，此时当结点B的平衡因子为-1时，可判断为LR型。
 **       虽然此时结点C的平衡因子的值可能为:-1, 0, 1. 
 **       但旋转处理的方式是一致的，只是旋转之后的平衡因子不一致.
 **注意事项: 
 **     1. 图(1)中A表示左边失衡的结点 图(2)表示平衡处理的结果
 **作    者: # Qifeng.zou # 2013.12.13 #
 ******************************************************************************/
int avl_lr_balance(avl_tree_t *tree, avl_node_t *node)
{
    avl_node_t *lchild = node->lchild,
        *parent = node->parent, *lrchild = NULL;


    lrchild = lchild->rchild;
    switch(lrchild->bf)
    {
        case AVL_LH:
        {
            node->bf = AVL_RH;
            lchild->bf = AVL_EH;
            lrchild->bf = AVL_EH;
            break;
        }
        case AVL_EH:
        {
           node->bf = AVL_EH;
           lchild->bf = AVL_EH;
           lrchild->bf = AVL_EH;
           break;
        }
        case AVL_RH:
        {
            node->bf = AVL_EH;
            lchild->bf = AVL_LH;
            lrchild->bf = AVL_EH;
            break;
        }
    }

    avl_set_rchild(lchild, lrchild->lchild);
    avl_set_lchild(lrchild, lchild);
    avl_set_lchild(node, lrchild->rchild);
    avl_set_rchild(lrchild, node); 

    avl_replace_child(tree, parent, node, lrchild);

    return AVL_SUCCESS;
}

/******************************************************************************
 **函数名称: avl_left_balance
 **功    能: 对左边失去平衡的结点进行平衡化处理(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 左边失去平衡的结点
 **输出参数: NONE
 **返    回: AVL_SUCCESS:成功 AVL_FAILED:失败
 **实现描述: 
 ** 场景1: LL型
 **              A                  B
 **             / \                / \
 **            B   C      ->      BL  A
 **           / \                /   / \
 **          BL BR              X   BR  C
 **         /
 **        X
 **             (1)                (2)
 ** 说明: 结点A是失衡结点，此时当结点B的平衡因子为1时，可判断为LL型。
 ** 场景2: LR型
 **              A                    C
 **             / \                /     \
 **            B  AR      ->      B       A
 **           / \                / \     / \
 **          BL  C              BL CL   CR AR
 **             / \
 **            CL CR
 **             (1)                (2)
 ** 说明: 结点A是失衡结点，此时当结点B的平衡因子为-1时，可判断为LR型。
 **       虽然此时结点C的平衡因子的值可能为:-1, 0, 1. 
 **       但旋转处理的方式是一致的，只是旋转之后的平衡因子不一致.
 **注意事项: 
 **     1. 图(1)中A表示左边失衡的结点 图(2)表示平衡处理的结果
 **作    者: # Qifeng.zou # 2013.12.13 #
 ******************************************************************************/
int avl_left_balance(avl_tree_t *tree, avl_node_t *node)
{
    avl_node_t *lchild = node->lchild;

    switch(lchild->bf)
    {
        case AVL_LH:    /* 场景1: LL型 */
        {
            return avl_ll_balance(tree, node);
        }
        case AVL_RH:    /* 场景2: LR型 */
        {
            return avl_lr_balance(tree, node);
        }
    }

    return AVL_FAILED;
}

/******************************************************************************
 **函数名称: avl_search
 **功    能: 查找指定的结点
 **输入参数: 
 **     tree: 平衡二叉树
 **     key: 需查找的关键字
 **输出参数: NONE
 **返    回: 结点地址
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.12 #
 ******************************************************************************/
const avl_node_t *avl_search(const avl_tree_t *tree, int key)
{
    const avl_node_t *node = tree->root;

    while (NULL != node)
    {
        if (node->key == key)
        {
            return node;
        }
        else if (key < node->key)
        {
            node = node->lchild;
        }
        else
        {
            node = node->rchild;
        }
    }

    return NULL; /* Didn't find */
}

/******************************************************************************
 **函数名称: avl_destroy
 **功    能: 销毁AVL树(对外接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **输出参数: NONE
 **返    回: void
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.15 #
 ******************************************************************************/
void avl_destroy(avl_tree_t **tree)
{
    if (NULL != (*tree)->root)
    {
        _avl_destroy((*tree)->root);
        (*tree)->root = NULL;
    }
    free(*tree);
    *tree = NULL;
}

/******************************************************************************
 **函数名称: avl_assert
 **功    能: 检测结点是否正常
 **输入参数: 
 **     node: 被检测的结点
 **输出参数: NONE
 **返    回: void
 **实现描述: 
 **注意事项: # 非常有效 #
 **作    者: # Qifeng.zou # 2013.12.20 #
 ******************************************************************************/
void avl_assert(const avl_node_t *node)
{
    if ((NULL == node)
        || (NULL == node->parent)) 
    {
        return;
    }

    if ((node->parent->lchild != node)
        && (node->parent->rchild != node)) 
    {
        assert(0);
    }

    if ((node->parent == node->lchild)
        || (node->parent == node->rchild))
    {
        assert(0);
    }
}

/******************************************************************************
 **函数名称: avl_delete
 **功    能: 删除key值结点(对外接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     key: 被删除的关键字
 **输出参数: 
 **     data: 附加数据
 **返    回: AVL_SUCCESS:成功 AVL_FAILED:失败
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.19 #
 ******************************************************************************/
int avl_delete(avl_tree_t *tree, int key, void **data)
{
    bool lower = false;

    if (NULL == tree->root)
    {
        return AVL_SUCCESS;
    }

    return _avl_delete(tree, tree->root, key, &lower, data);
}

/******************************************************************************
 **函数名称: _avl_delete
 **功    能: 搜索并删除指定的key值结点(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 以node为根结点的子树
 **     key: 被删除的关键字
 **输出参数: 
 **     lower: 高度是否降低
 **     data: 附加数据
 **返    回: AVL_SUCCESS:成功 AVL_FAILED:失败
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.19 #
 ******************************************************************************/
static int _avl_delete(avl_tree_t *tree, avl_node_t *node, int key, bool *lower, void **data)
{
    avl_node_t *parent = node->parent;

    /* 1. 查找需要被删除的结点 */
    if (key < node->key)         /* 左子树上查找 */
    {
        if (NULL == node->lchild)
        {
            return AVL_SUCCESS;
        }
        
        _avl_delete(tree, node->lchild, key, lower, data);
        avl_assert(node);
        avl_assert(node->lchild);
        if (true == *lower)
        {
            return avl_delete_left_balance(tree, node, lower);
        }
        return AVL_SUCCESS;
    }
    else if (key > node->key)    /* 右子树上查找 */
    {
        if (NULL == node->rchild)
        {
            return AVL_SUCCESS;
        }
        
        _avl_delete(tree, node->rchild, key, lower, data);
        avl_assert(node);
        avl_assert(node->rchild);
        if (true == *lower)
        {
            return avl_delete_right_balance(tree, node, lower);
        }
        return AVL_SUCCESS;
    }

    /* 2. 已找到将被删除的结点node */
    *data = node->data;

    /* 2.1 右子树为空, 只需接它的左子树(叶子结点也走这) */
    if (NULL == node->rchild)
    {
        *lower = true;

        avl_replace_child(tree, parent, node, node->lchild);

        avl_assert(parent);
        avl_assert(node->lchild);
        free(node), node = NULL;
        return AVL_SUCCESS;
    }
    /* 2.2 左子树空, 只需接它的右子树 */
    else if (NULL == node->lchild)
    {
        *lower = true;

        avl_replace_child(tree, parent, node, node->rchild)

        avl_assert(parent);
        avl_assert(node->rchild);
        free(node), node=NULL;
        return AVL_SUCCESS;
    }

    /* 2.3 左右子树均不为空: 查找左子树最右边的结点 替换被删的结点 */
    avl_replace_and_delete(tree, node, node->lchild, lower);
    if (true == *lower)
    {
        avl_assert(node);
        return avl_delete_left_balance(tree, node, lower);
    }
    return AVL_SUCCESS;
}

/******************************************************************************
 **函数名称: avl_replace_and_delete
 **功    能: 找到替换结点, 并替换被删除的结点(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 将被删除的结点
 **     prev: 前继结点:此结点最右端的结点将会用来替换被删除的结点.
 **输出参数: 
 **     lower: 高度是否变化
 **返    回: AVL_SUCCESS:成功 AVL_FAILED:失败
 **实现描述: 
 **注意事项: 
 **     >> 在此其实并不会删除node, 而是将replace的值给了node, 再删了replace.
 **         因为在此使用的递归算法, 如果真把node给释放了，会造成压栈的信息出现错误!
 **作    者: # Qifeng.zou # 2013.12.19 #
 ******************************************************************************/
int avl_replace_and_delete(avl_tree_t *tree,
        avl_node_t *node, avl_node_t *prev, bool *lower)
{
    if (NULL == prev->rchild)
    {
        *lower = true;
        
        node->key = prev->key;    /* 注: 将rnode的值给了dnode */
        node->data = prev->data;
        if (prev == node->lchild)
        {
            avl_set_lchild(node, prev->lchild);
            /* prev->parent == dnode结点可能失衡，此处理交给前栈的函数处理 */
        }
        else
        {
            avl_set_rchild(prev->parent, prev->lchild);
            /* rnode的父结点可能失衡，此处理交给前栈的函数处理 */
        }
        avl_assert(node);
        avl_assert(prev->parent);
        avl_assert(prev->lchild);
        free(prev), prev=NULL;    /* 注意: 释放的不是dnode, 而是rnode */
        return AVL_SUCCESS;
    }

    avl_replace_and_delete(tree, node, prev->rchild, lower);
    if (true == *lower)
    {
        avl_assert(node);
        avl_assert(prev);
        /* dnode的父结点可能失衡，此处理交给前栈的函数处理
            但replace可能失衡，因此必须在此自己处理 */
        avl_delete_right_balance(tree, prev, lower);
    }
    return AVL_SUCCESS;
}

/******************************************************************************
 **函数名称: avl_delete_left_balance
 **功    能: 结点node的左子树某结点被删除, 左高度降低后, 平衡化处理(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 结点node的左子树的某个结点已被删除
 **输出参数: 
 **     lower: 高度是否变化
 **返    回: AVL_SUCCESS:成功 AVL_FAILED:失败
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.19 #
 ******************************************************************************/
int avl_delete_left_balance(avl_tree_t *tree, avl_node_t *node, bool *lower)
{
    avl_node_t *rchild = NULL, *rlchild = NULL, *parent = node->parent;

    switch(node->bf)
    {
        case AVL_LH:    /* 左高: 左子树高度减1 树变矮 */
        {
            node->bf = AVL_EH;
            *lower = true;
            break;
        }
        case AVL_EH:    /* 等高: 左子树高度减1 树高度不变 */
        {
            node->bf = AVL_RH;
            *lower = false;
            break;
        }
        case AVL_RH:    /* 右高: 左子树高度减1 树失去平衡 */
        {
            rchild = node->rchild;
            switch(rchild->bf)
            {
                case AVL_EH:    /* RR型: 向左旋转 */
                case AVL_RH:    /* RR型: 向左旋转 */
                {
                    if (AVL_EH == rchild->bf)
                    {
                        *lower = false;
                        rchild->bf = AVL_LH;
                        node->bf = AVL_RH;
                    }
                    else if (AVL_RH == rchild->bf)
                    {
                        *lower = true;
                        rchild->bf = AVL_EH;
                        node->bf = AVL_EH;
                    }

                    avl_set_rchild(node, rchild->lchild);
                    avl_set_lchild(rchild, node);
                    avl_replace_child(tree, parent, node, rchild);

                    avl_assert(node);
                    avl_assert(rchild);
                    break;
                }
                case AVL_LH:    /* RL型: 先向右旋转 再向左旋转 */
                {
                    *lower = true;
                    rlchild = rchild->lchild;
                    switch(rlchild->bf)
                    {
                        case AVL_LH:
                        {
                            node->bf = AVL_EH;
                            rchild->bf = AVL_RH;
                            rlchild->bf = AVL_EH;
                            break;
                        }
                        case AVL_EH:
                        {
                            node->bf = AVL_EH;
                            rchild->bf = AVL_EH;
                            rlchild->bf = AVL_EH;
                            break;
                        }
                        case AVL_RH:
                        {
                            node->bf = AVL_LH;
                            rchild->bf = AVL_EH;
                            rlchild->bf = AVL_EH;
                            break;
                        }
                    }

                    avl_set_rchild(node, rlchild->lchild);
                    avl_set_lchild(rchild, rlchild->rchild);
                    avl_set_lchild(rlchild, node);
                    avl_set_rchild(rlchild, rchild); 

                    avl_replace_child(tree, parent, node, rlchild);

                    avl_assert(parent);
                    avl_assert(node);
                    avl_assert(rchild);
                    avl_assert(rlchild);
                    break;
                }
            }
            break;
        }
    }

    return AVL_SUCCESS;
}

/******************************************************************************
 **函数名称: avl_delete_right_balance
 **功    能: 结点node的右子树某结点被删除, 左高度降低后, 平衡化处理(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **     node: 结点node的右子树的某个结点已被删除
 **输出参数: 
 **     lower: 高度是否变化
 **返    回: AVL_SUCCESS:成功 AVL_FAILED:失败
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.19 #
 ******************************************************************************/
int avl_delete_right_balance(avl_tree_t *tree, avl_node_t *node, bool *lower)
{
    avl_node_t *lchild = NULL, *lrchild = NULL, *parent = node->parent;

    switch(node->bf)
    {
        case AVL_LH:    /* 左高: 右子树高度减1 树失去平衡 */
        {
            lchild = node->lchild;
            switch(lchild->bf)
            {
                case AVL_EH:    /* LL型: 向右旋转 */
                case AVL_LH:    /* LL型: 向右旋转 */
                {
                    if (AVL_EH == lchild->bf)
                    {
                        *lower = false;
                        lchild->bf = AVL_RH;
                        node->bf = AVL_LH;
                    }
                    else
                    {
                        *lower = true;
                        lchild->bf = AVL_EH;
                        node->bf = AVL_EH;
                    }

                    avl_set_lchild(node, lchild->rchild);
                    avl_set_rchild(lchild, node); 
                    avl_replace_child(tree, parent, node, lchild);

                    avl_assert(parent);
                    avl_assert(node);
                    avl_assert(lchild);
                    break;
                }
                case AVL_RH:    /* LR型: 先向左旋转 再向右旋转 */
                {
                    *lower = true;
                    lrchild = lchild->rchild;
                    switch(lrchild->bf)
                    {
                        case AVL_LH:
                        {
                            node->bf = AVL_RH;
                            lchild->bf = AVL_EH;
                            lrchild->bf = AVL_EH;
                            break;
                        }
                        case AVL_EH:
                        {
                            node->bf = AVL_EH;
                            lchild->bf = AVL_EH;
                            lrchild->bf = AVL_EH;
                            break;
                        }
                        case AVL_RH:
                        {
                            node->bf = AVL_EH;
                            lchild->bf = AVL_LH;
                            lrchild->bf = AVL_EH;
                            break;
                        }
                    }

                    avl_set_lchild(node, lrchild->rchild);
                    avl_set_rchild(lchild, lrchild->lchild);
                    avl_set_rchild(lrchild, node);
                    avl_set_lchild(lrchild, lchild);

                    avl_replace_child(tree, parent, node, lrchild);

                    avl_assert(parent);
                    avl_assert(node);
                    avl_assert(lchild);
                    avl_assert(lrchild);
                    break;
                }
            }
            break;
        }
        case AVL_EH:    /* 等高: 右子树高度减1 树高度不变 */
        {
            node->bf = AVL_LH;
            *lower = false;
            break;
        }
        case AVL_RH:    /* 右高: 右子树高度减1 树变矮 */
        {
            node->bf = AVL_EH;
            *lower = true;
            break;
        }
    }
    return AVL_SUCCESS;
}
/******************************************************************************
 **函数名称: _avl_destroy
 **功    能: 销毁AVL树(内部接口)
 **输入参数: 
 **     node: 需要被销毁的结点
 **输出参数: NONE
 **返    回: void
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.15 #
 ******************************************************************************/
static void _avl_destroy(avl_node_t *node)
{
    if (NULL != node->lchild)
    {
        _avl_destroy(node->lchild);
    }

    if (NULL != node->rchild)
    {
        _avl_destroy(node->rchild);
    }

    free(node);
}

int _avl_print(avl_node_t *node, Stack_t *stack);
/******************************************************************************
 **函数名称: val_print
 **功    能: 打印平衡二叉树(外部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **输出参数: NONE
 **返    回: void
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.17 #
 ******************************************************************************/
int avl_print(avl_tree_t *tree)
{
    int ret = 0;
    Stack_t stack;

    memset(&stack, 0, sizeof(stack));

    if (NULL == tree->root)
    {
        return AVL_SUCCESS;
    }

    ret = stack_init(&stack, AVL_MAX_DEPTH);
    if (AVL_SUCCESS != ret)
    {
        return AVL_ERR_STACK;
    }
    
    _avl_print(tree->root, &stack);

    stack_destroy(&stack);
    return AVL_SUCCESS;
}

/******************************************************************************
 **函数名称: avl_print_head
 **功    能: 打印结点头(内部接口)
 **输入参数: 
 **     node: 被打印的结点
 **     depth: 结点深度
 **输出参数: NONE
 **返    回: void
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.17 #
 ******************************************************************************/
void avl_print_head(avl_node_t *node, int depth)
{
    int idx = 0;
    avl_node_t *parent = node->parent;
    
    while (depth > 1)
    {
        depth--;
        if (1 == depth)
        {
            fprintf(stderr, "|");
            for (idx=0; idx<8; idx++)
            {
                if (0 == idx)
                {
                    if (parent->lchild == node)
                    {
                        fprintf(stderr, "l");
                    }
                    else
                    {
                        fprintf(stderr, "r");
                    }
                }
                else
                {
                    fprintf(stderr, "-");
                }
            }
        }
        else
        {
			fprintf(stderr, "|");
			for (idx=0; idx<8; idx++)
			{
				fprintf(stderr, " ");
			}
        }
    }
    
    if ((NULL == node->lchild)
        && (NULL == node->rchild))
    {
        fprintf(stderr, "<%03d:%d/>\n", node->key, node->bf);
    }
    else
    {
        fprintf(stderr, "<%03d:%d>\n", node->key, node->bf);
    }
}

/******************************************************************************
 **函数名称: avl_print_tail
 **功    能: 打印结点尾(内部接口)
 **输入参数: 
 **     node: 被打印的结点
 **     depth: 结点深度
 **输出参数: NONE
 **返    回: void
 **实现描述: 
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.17 #
 ******************************************************************************/
void avl_print_tail(avl_node_t *node, int depth)
{
    int idx = 0;

    if ((NULL == node->lchild)
        && (NULL == node->rchild))
    {
        return;
    }

    while (depth > 1)
    {
        fprintf(stderr, "|");
		for (idx=0; idx<8; idx++)
		{
			fprintf(stderr, " ");
		}
        depth--;
    }

    fprintf(stderr, "</%03d>\n", node->key);
}

/******************************************************************************
 **函数名称: _avl_print
 **功    能: 打印平衡二叉树(内部接口)
 **输入参数: 
 **     tree: 平衡二叉树
 **输出参数: NONE
 **返    回: void
 **实现描述: 
 **     1. 结点入栈 
 **     2. 打印该结点(头)
 **     3. 打印该结点的右子树
 **     4. 打印该结点的左子树
 **     5. 打印该结点(尾)
 **     6. 结点出栈
 **注意事项: 
 **作    者: # Qifeng.zou # 2013.12.17 #
 ******************************************************************************/
int _avl_print(avl_node_t *root, Stack_t *stack)
{
    int depth = 0;
    avl_node_t *node = root;


    /* 1. 将要处理的结点压栈 */
    stack_push(stack, node);
    
    depth = stack_depth(stack);

    /* 2. 打印结点头 */
    avl_print_head(node, depth);

    /* 3. 打印右子树 */
    if (NULL != node->rchild)
    {
        _avl_print(node->rchild, stack);
    }

    /* 4. 打印左子树 */
    if (NULL != node->lchild)
    {
        _avl_print(node->lchild, stack);
    }

    depth = stack_depth(stack);

    /* 5. 打印结点尾 */
    avl_print_tail(node, depth);    

    /* 6. 出栈 */
    stack_pop(stack);
    return AVL_SUCCESS;
}