#include <stdio.h>
#include <stdlib.h>

#define ORDER 5  // B+树的阶数

typedef struct BPlusTreeNode
{
    int keys[ORDER - 1];                    // 索引键
    struct BPlusTreeNode* children[ORDER];  // 指向子节点的指针
    struct BPlusTreeNode* next;             // 叶子节点之间的链表
    int numKeys;                            // 当前节点中的键数
    int isLeaf;                             // 是否为叶子节点
} BPlusTreeNode;

typedef struct BPlusTree
{
    BPlusTreeNode* root;
} BPlusTree;

// 创建一个新节点
BPlusTreeNode* createNode(int isLeaf)
{
    BPlusTreeNode* node = (BPlusTreeNode*)malloc(sizeof(BPlusTreeNode));
    node->isLeaf = isLeaf;
    node->numKeys = 0;
    node->next = NULL;
    for (int i = 0; i < ORDER; i++)
    {
        node->children[i] = NULL;
    }
    return node;
}

// 初始化B+树
BPlusTree* createBPlusTree()
{
    BPlusTree* tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    tree->root = createNode(1);  // 初始根节点为空的叶子节点
    return tree;
}

// 在节点中搜索键
int searchKey(BPlusTreeNode* node, int key)
{
    int i = 0;
    while (i < node->numKeys && key > node->keys[i])
    {
        i++;
    }
    if (node->isLeaf)
    {
        if (i < node->numKeys && key == node->keys[i])
        {
            return 1;  // 找到键
        }
        else
        {
            return 0;  // 没有找到
        }
    }
    else
    {
        return searchKey(node->children[i], key);
    }
}

// 将一个满的节点分裂成两个
void splitChild(BPlusTreeNode* parent, int index, BPlusTreeNode* child)
{
    BPlusTreeNode* newNode = createNode(child->isLeaf);
    newNode->numKeys = ORDER / 2;

    for (int i = 0; i < ORDER / 2; i++)
    {
        newNode->keys[i] = child->keys[i + ORDER / 2];
    }
    if (!child->isLeaf)
    {
        for (int i = 0; i <= ORDER / 2; i++)
        {
            newNode->children[i] = child->children[i + ORDER / 2];
        }
    }

    child->numKeys = ORDER / 2;

    for (int i = parent->numKeys; i >= index + 1; i--)
    {
        parent->children[i + 1] = parent->children[i];
    }
    parent->children[index + 1] = newNode;

    for (int i = parent->numKeys - 1; i >= index; i--)
    {
        parent->keys[i + 1] = parent->keys[i];
    }
    parent->keys[index] = child->keys[ORDER / 2 - 1]; // Correctly update the parent key
    parent->numKeys++;
}

// 向非满节点插入键
void insertNonFull(BPlusTreeNode* node, int key)
{
    int i = node->numKeys - 1;
    if (node->isLeaf)
    {
        while (i >= 0 && key < node->keys[i])
        {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->numKeys++;
    }
    else
    {
        while (i >= 0 && key < node->keys[i])
        {
            i--;
        }
        i++;
        if (node->children[i]->numKeys == ORDER - 1)
        {
            splitChild(node, i, node->children[i]);
            if (key > node->keys[i])
            {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

// 插入新键
void insert(BPlusTree* tree, int key)
{
    BPlusTreeNode* root = tree->root;
    if (root->numKeys == ORDER - 1)
    {
        BPlusTreeNode* newRoot = createNode(0);
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);
        int i = 0;
        if (newRoot->keys[0] < key)
        {
            i++;
        }
        insertNonFull(newRoot->children[i], key);
        tree->root = newRoot;
    }
    else
    {
        insertNonFull(root, key);
    }
}

// 删除叶子节点中的键
void removeFromLeaf(BPlusTreeNode* node, int index)
{
    for (int i = index + 1; i < node->numKeys; i++)
    {
        node->keys[i - 1] = node->keys[i];
    }
    node->numKeys--;
}

// 删除非叶子节点中的键
void removeFromNonLeaf(BPlusTreeNode* node, int index)
{
    int key = node->keys[index];
    if (node->children[index]->numKeys >= ORDER / 2)
    {
        int pred = node->children[index]->keys[node->children[index]->numKeys - 1];
        node->keys[index] = pred;
        deleteKey(node->children[index], pred);
    }
    else if (node->children[index + 1]->numKeys >= ORDER / 2)
    {
        int succ = node->children[index + 1]->keys[0];
        node->keys[index] = succ;
        deleteKey(node->children[index + 1], succ);
    }
    else
    {
        // 合并节点
        BPlusTreeNode* child = node->children[index];
        BPlusTreeNode* sibling = node->children[index + 1];
        child->keys[child->numKeys] = key;
        child->numKeys++;
        for (int i = 0; i < sibling->numKeys; i++)
        {
            child->keys[child->numKeys] = sibling->keys[i];
            child->numKeys++;
        }
        if (!child->isLeaf)
        {
            for (int i = 0; i <= sibling->numKeys; i++)
            {
                child->children[child->numKeys + i] = sibling->children[i];
            }
        }
        for (int i = index + 1; i < node->numKeys; i++)
        {
            node->keys[i - 1] = node->keys[i];
        }
        for (int i = index + 2; i <= node->numKeys; i++)
        {
            node->children[i - 1] = node->children[i];
        }
        child->numKeys += sibling->numKeys;
        node->numKeys--;
        free(sibling);
    }
}

// 删除操作的主函数
void deleteKey(BPlusTreeNode* node, int key)
{
    int index = 0;
    while (index < node->numKeys && node->keys[index] < key)
    {
        index++;
    }

    if (index < node->numKeys && node->keys[index] == key)
    {
        if (node->isLeaf)
        {
            removeFromLeaf(node, index);
        }
        else
        {
            removeFromNonLeaf(node, index);
        }
    }
    else
    {
        if (node->isLeaf)
        {
            printf("%d未找到\n", key);
            return;
        }

        int flag = (index == node->numKeys);
        if (node->children[index]->numKeys < ORDER / 2)
        {
            if (index != 0 && node->children[index - 1]->numKeys >= ORDER / 2)
            {
                BPlusTreeNode* child = node->children[index];
                BPlusTreeNode* leftSibling = node->children[index - 1];

                // 从左兄弟借用
                for (int i = child->numKeys - 1; i >= 0; i--)
                {
                    child->keys[i + 1] = child->keys[i];
                }
                child->keys[0] = node->keys[index - 1];
                node->keys[index - 1] = leftSibling->keys[leftSibling->numKeys - 1];
                child->numKeys++;
                leftSibling->numKeys--;
            }
            else if (index != node->numKeys && node->children[index + 1]->numKeys >= ORDER / 2)
            {
                BPlusTreeNode* child = node->children[index];
                BPlusTreeNode* rightSibling = node->children[index + 1];

                // 从右兄弟借用
                child->keys[child->numKeys] = node->keys[index];
                node->keys[index] = rightSibling->keys[0];
                child->numKeys++;
                for (int i = 1; i < rightSibling->numKeys; i++)
                {
                    rightSibling->keys[i - 1] = rightSibling->keys[i];
                }
                rightSibling->numKeys--;
            }
            else
            {
                // 合并逻辑
                if (index != node->numKeys)
                {
                    BPlusTreeNode* child = node->children[index];
                    BPlusTreeNode* sibling = node->children[index + 1];
                    child->keys[child->numKeys] = node->keys[index];
                    child->numKeys++;
                    for (int i = 0; i < sibling->numKeys; i++)
                    {
                        child->keys[child->numKeys + i] = sibling->keys[i];
                    }
                    if (!child->isLeaf)
                    {
                        for (int i = 0; i <= sibling->numKeys; i++)
                        {
                            child->children[child->numKeys + i] = sibling->children[i];
                        }
                    }
                    child->numKeys += sibling->numKeys;
                    for (int i = index + 1; i < node->numKeys; i++)
                    {
                        node->keys[i - 1] = node->keys[i];
                    }
                    for (int i = index + 2; i <= node->numKeys; i++)
                    {
                        node->children[i - 1] = node->children[i];
                    }
                    node->numKeys--;
                    free(sibling);
                }
                else
                {
                    BPlusTreeNode* child = node->children[index - 1];
                    BPlusTreeNode* sibling = node->children[index];
                    child->keys[child->numKeys] = node->keys[index - 1];
                    child->numKeys++;
                    for (int i = 0; i < sibling->numKeys; i++)
                    {
                        child->keys[child->numKeys + i] = sibling->keys[i];
                    }
                    if (!child->isLeaf)
                    {
                        for (int i = 0; i <= sibling->numKeys; i++)
                        {
                            child->children[child->numKeys + i] = sibling->children[i];
                        }
                    }
                    child->numKeys += sibling->numKeys;
                    for (int i = index; i < node->numKeys; i++)
                    {
                        node->keys[i - 1] = node->keys[i];
                    }
                    for (int i = index + 1; i <= node->numKeys; i++)
                    {
                        node->children[i - 1] = node->children[i];
                    }
                    node->numKeys--;
                    free(sibling);
                }
            }
        }
        if (flag && index > node->numKeys)
        {
            deleteKey(node->children[index - 1], key);
        }
        else
        {
            deleteKey(node->children[index], key);
        }
    }
}

// 从B+树中删除键
void deleteFromTree(BPlusTree* tree, int key)
{
    deleteKey(tree->root, key);
    if (tree->root->numKeys == 0)
    {
        BPlusTreeNode* oldRoot = tree->root;
        if (!tree->root->isLeaf)
        {
            tree->root = tree->root->children[0];
        }
        else
        {
            tree->root = NULL;
        }
        free(oldRoot);
    }

}
int wx(BPlusTreeNode* node)
{

}

// 打印B+树
void printBPlusTree(BPlusTreeNode* node, int level)
{
    if (node)
    {
        printf("Level %d: ", level);
        for (int i = 0; i < node->numKeys; i++)
        {
            printf("%d ", node->keys[i]);
        }
        printf("\n");
        if (!node->isLeaf)
        {
            for (int i = 0; i <= node->numKeys; i++)
            {
                printBPlusTree(node->children[i], level + 1);
            }
        }
    }
}
int weihu(BPlusTreeNode* node)
{
    int i;
    if (!node->isLeaf)
    {
        for ( i = 0; i < node->numKeys; i++)
        {
            node->keys[i]=weihu(node->children[i]);
        }
}
return node->keys[(node->numKeys)-1];
}

// 主程序
int main()
{
    BPlusTree* tree = createBPlusTree();
    srand(time(NULL));
    // 插入测试数据
    int key,flag;
    for (int i = 0; i <15; i++)
    {
        key=rand()%10000;
        insert(tree, key);
    }

    while(1)
    {
        printf(" 0）退出\n 1）打印\n 2）搜索\n 3）插入\n 4）删除\n");
        scanf("%d",&flag);
        if(flag==0)
            break;
        switch(flag)
        {
        case 1:
            printf("B+树结构为:\n");
            printBPlusTree(tree->root, 0);
            break;
        case 2:
            printf("请输入需要搜索的数字\n");
            scanf("%d",&key);
            printf(" %s\n", searchKey(tree->root, key) ? "已找到" : "未找到");
            break;
        case 3:
            printf("请输入需要插入的数字\n");
            scanf("%d",&key);
            insert(tree, key);
            printBPlusTree(tree->root, 0);
            break;
        case 4:
            printf("请输入需要删除的数字\n");
            scanf("%d",&key);
            // 删除测试
            deleteFromTree(tree, key);
            weihu(tree->root);
            printBPlusTree(tree->root, 0);
            break;
        }
    }
    return 0;
}
