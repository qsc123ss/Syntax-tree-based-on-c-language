#include <stdio.h>
#include <stdlib.h>

#define ORDER 5  // B+���Ľ���

typedef struct BPlusTreeNode
{
    int keys[ORDER - 1];                    // ������
    struct BPlusTreeNode* children[ORDER];  // ָ���ӽڵ��ָ��
    struct BPlusTreeNode* next;             // Ҷ�ӽڵ�֮�������
    int numKeys;                            // ��ǰ�ڵ��еļ���
    int isLeaf;                             // �Ƿ�ΪҶ�ӽڵ�
} BPlusTreeNode;

typedef struct BPlusTree
{
    BPlusTreeNode* root;
} BPlusTree;

// ����һ���½ڵ�
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

// ��ʼ��B+��
BPlusTree* createBPlusTree()
{
    BPlusTree* tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    tree->root = createNode(1);  // ��ʼ���ڵ�Ϊ�յ�Ҷ�ӽڵ�
    return tree;
}

// �ڽڵ���������
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
            return 1;  // �ҵ���
        }
        else
        {
            return 0;  // û���ҵ�
        }
    }
    else
    {
        return searchKey(node->children[i], key);
    }
}

// ��һ�����Ľڵ���ѳ�����
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

// ������ڵ�����
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

// �����¼�
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

// ɾ��Ҷ�ӽڵ��еļ�
void removeFromLeaf(BPlusTreeNode* node, int index)
{
    for (int i = index + 1; i < node->numKeys; i++)
    {
        node->keys[i - 1] = node->keys[i];
    }
    node->numKeys--;
}

// ɾ����Ҷ�ӽڵ��еļ�
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
        // �ϲ��ڵ�
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

// ɾ��������������
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
            printf("%dδ�ҵ�\n", key);
            return;
        }

        int flag = (index == node->numKeys);
        if (node->children[index]->numKeys < ORDER / 2)
        {
            if (index != 0 && node->children[index - 1]->numKeys >= ORDER / 2)
            {
                BPlusTreeNode* child = node->children[index];
                BPlusTreeNode* leftSibling = node->children[index - 1];

                // �����ֵܽ���
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

                // �����ֵܽ���
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
                // �ϲ��߼�
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

// ��B+����ɾ����
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

// ��ӡB+��
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

// ������
int main()
{
    BPlusTree* tree = createBPlusTree();
    srand(time(NULL));
    // �����������
    int key,flag;
    for (int i = 0; i <15; i++)
    {
        key=rand()%10000;
        insert(tree, key);
    }

    while(1)
    {
        printf(" 0���˳�\n 1����ӡ\n 2������\n 3������\n 4��ɾ��\n");
        scanf("%d",&flag);
        if(flag==0)
            break;
        switch(flag)
        {
        case 1:
            printf("B+���ṹΪ:\n");
            printBPlusTree(tree->root, 0);
            break;
        case 2:
            printf("��������Ҫ����������\n");
            scanf("%d",&key);
            printf(" %s\n", searchKey(tree->root, key) ? "���ҵ�" : "δ�ҵ�");
            break;
        case 3:
            printf("��������Ҫ���������\n");
            scanf("%d",&key);
            insert(tree, key);
            printBPlusTree(tree->root, 0);
            break;
        case 4:
            printf("��������Ҫɾ��������\n");
            scanf("%d",&key);
            // ɾ������
            deleteFromTree(tree, key);
            weihu(tree->root);
            printBPlusTree(tree->root, 0);
            break;
        }
    }
    return 0;
}
