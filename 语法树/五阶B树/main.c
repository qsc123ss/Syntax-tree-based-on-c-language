#include <stdio.h>
#include <stdlib.h>

#define ORDER 5  // B���Ľ���

typedef struct BTreeNode {
    int keys[ORDER - 1];
    struct BTreeNode* children[ORDER];
    int numKeys;
    int isLeaf;
} BTreeNode;

typedef struct BTree {
    BTreeNode* root;
} BTree;

// ��������
BTreeNode* createNode(int isLeaf);
BTree* createBTree();
void freeBTree(BTreeNode* node);
int searchKey(BTreeNode* node, int key);
void deleteKey(BTreeNode* node, int key);
void insert(BTree* tree, int key);
void splitChild(BTreeNode* parent, int index, BTreeNode* child);
void insertNonFull(BTreeNode* node, int key);
void printBTree(BTreeNode* node, int depth);

// �����µĽڵ�
BTreeNode* createNode(int isLeaf) {
    BTreeNode* node = (BTreeNode*)malloc(sizeof(BTreeNode));
    node->isLeaf = isLeaf;
    node->numKeys = 0;
    for (int i = 0; i < ORDER; i++) {
        node->children[i] = NULL;
    }
    return node;
}

// ��ʼ��B��
BTree* createBTree() {
    BTree* tree = (BTree*)malloc(sizeof(BTree));
    tree->root = createNode(1);  // ��ʼʱ���ڵ�Ϊ��Ҷ�ӽڵ�
    return tree;
}

// �ͷ�B��
void freeBTree(BTreeNode* node) {
    if (node) {
        for (int i = 0; i <= node->numKeys; i++) {
            freeBTree(node->children[i]);
        }
        free(node);
    }
}

// �����ڵ��еļ�
int searchKey(BTreeNode* node, int key) {
    int i = 0;
    while (i < node->numKeys && key > node->keys[i]) {
        i++;
    }
    if (i < node->numKeys && key == node->keys[i]) {
        return 1;  // �ҵ���
    }
    if (node->isLeaf) {
        return 0;  // û�ҵ���
    }
    return searchKey(node->children[i], key);
}

// ��ȡǰ���ڵ�ļ�
int getPredecessor(BTreeNode* node, int idx) {
    BTreeNode* current = node->children[idx];
    while (!current->isLeaf) {
        current = current->children[current->numKeys];
    }
    return current->keys[current->numKeys - 1];
}

// ��ȡ��̽ڵ�ļ�
int getSuccessor(BTreeNode* node, int idx) {
    BTreeNode* current = node->children[idx + 1];
    while (!current->isLeaf) {
        current = current->children[0];
    }
    return current->keys[0];
}

// �ϲ��ڵ�
void merge(BTreeNode* node, int idx) {
    BTreeNode* child = node->children[idx];
    BTreeNode* sibling = node->children[idx + 1];

    child->keys[ORDER / 2 - 1] = node->keys[idx];
    for (int i = 0; i < sibling->numKeys; ++i) {
        child->keys[i + ORDER / 2] = sibling->keys[i];
    }

    if (!child->isLeaf) {
        for (int i = 0; i <= sibling->numKeys; ++i) {
            child->children[i + ORDER / 2] = sibling->children[i];
        }
    }

    child->numKeys += sibling->numKeys + 1;
    node->numKeys--;

    for (int i = idx + 1; i < node->numKeys; ++i) {
        node->keys[i - 1] = node->keys[i];
    }
    for (int i = idx + 2; i <= node->numKeys; ++i) {
        node->children[i - 1] = node->children[i];
    }

    free(sibling);
}

// �ӽڵ���ɾ����
void deleteFromNode(BTreeNode* node, int key, int idx) {
    int k = node->keys[idx];
    if (node->isLeaf) {
        for (int i = idx + 1; i < node->numKeys; ++i) {
            node->keys[i - 1] = node->keys[i];
        }
        node->numKeys--;
    } else {
        if (node->children[idx]->numKeys >= ORDER / 2) {
            int pred = getPredecessor(node, idx);
            node->keys[idx] = pred;
            deleteKey(node->children[idx], pred);
        } else if (node->children[idx + 1]->numKeys >= ORDER / 2) {
            int succ = getSuccessor(node, idx);
            node->keys[idx] = succ;
            deleteKey(node->children[idx + 1], succ);
        } else {
            merge(node, idx);
            deleteKey(node->children[idx], k);
        }
    }
}

// ɾ������������
void deleteKey(BTreeNode* node, int key) {
    int idx = 0;
    while (idx < node->numKeys && node->keys[idx] < key) {
        idx++;
    }
    if (idx < node->numKeys && node->keys[idx] == key) {
        deleteFromNode(node, key, idx);
    } else if (!node->isLeaf) {
        int flag = (idx == node->numKeys);
        if (node->children[idx]->numKeys < ORDER / 2) {
            if (idx != 0 && node->children[idx - 1]->numKeys >= ORDER / 2) {
                BTreeNode* child = node->children[idx];
                BTreeNode* sibling = node->children[idx - 1];

                for (int i = child->numKeys - 1; i >= 0; --i) {
                    child->keys[i + 1] = child->keys[i];
                }
                if (!child->isLeaf) {
                    for (int i = child->numKeys; i >= 0; --i) {
                        child->children[i + 1] = child->children[i];
                    }
                }
                child->keys[0] = node->keys[idx - 1];
                if (!node->isLeaf) {
                    child->children[0] = sibling->children[sibling->numKeys];
                }
                node->keys[idx - 1] = sibling->keys[sibling->numKeys - 1];
                child->numKeys++;
                sibling->numKeys--;
            } else if (idx != node->numKeys && node->children[idx + 1]->numKeys >= ORDER / 2) {
                BTreeNode* child = node->children[idx];
                BTreeNode* sibling = node->children[idx + 1];

                child->keys[child->numKeys] = node->keys[idx];
                if (!child->isLeaf) {
                    child->children[child->numKeys + 1] = sibling->children[0];
                }
                node->keys[idx] = sibling->keys[0];
                for (int i = 1; i < sibling->numKeys; ++i) {
                    sibling->keys[i - 1] = sibling->keys[i];
                }
                if (!sibling->isLeaf) {
                    for (int i = 1; i <= sibling->numKeys; ++i) {
                        sibling->children[i - 1] = sibling->children[i];
                    }
                }
                child->numKeys++;
                sibling->numKeys--;
            } else {
                if (idx != node->numKeys) {
                    merge(node, idx);
                } else {
                    merge(node, idx - 1);
                }
            }
        }
        if (flag && idx > node->numKeys) {
            deleteKey(node->children[idx - 1], key);
        } else {
            deleteKey(node->children[idx], key);
        }
    }
}

// ��������ڵ�
void insertNonFull(BTreeNode* node, int key) {
    int i = node->numKeys - 1;
    if (node->isLeaf) {
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->numKeys++;
    } else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->numKeys == ORDER - 1) {
            splitChild(node, i, node->children[i]);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

// ���ڵ���ѳ�����
void splitChild(BTreeNode* parent, int index, BTreeNode* child) {
    BTreeNode* newNode = createNode(child->isLeaf);
    newNode->numKeys = ORDER / 2 - 1;

    for (int j = 0; j < ORDER / 2 - 1; j++) {
        newNode->keys[j] = child->keys[j + ORDER / 2];
    }
    if (!child->isLeaf) {
        for (int j = 0; j < ORDER / 2; j++) {
            newNode->children[j] = child->children[j + ORDER / 2];
        }
    }
    child->numKeys = ORDER / 2 - 1;

    for (int j = parent->numKeys; j >= index + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[index + 1] = newNode;

    for (int j = parent->numKeys - 1; j >= index; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }
    parent->keys[index] = child->keys[ORDER / 2 - 1];
    parent->numKeys++;
}

// ����ڵ�
void insert(BTree* tree, int key) {
    BTreeNode* root = tree->root;
    if (root->numKeys == ORDER - 1) {
        BTreeNode* newRoot = createNode(0);
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);
        int i = 0;
        if (newRoot->keys[0] < key) {
            i++;
        }
        insertNonFull(newRoot->children[i], key);
        tree->root = newRoot;
    } else {
        insertNonFull(root, key);
    }
}

// ��ӡB��
void printBTree(BTreeNode* node, int depth) {
    if (node) {
        for (int i = 0; i < depth; i++) {
            printf("  ");
        }
        for (int i = 0; i < node->numKeys; i++) {
            printf("%d ", node->keys[i]);
        }
        printf("\n");
        if (!node->isLeaf) {
            for (int i = 0; i <= node->numKeys; i++) {
                printBTree(node->children[i], depth + 1);
            }
        }
    }
}

int main() {
    BTree* tree = createBTree();
    srand(time(NULL));
    // �����������;
    int flag,k;
    // �����������
    int keys[] = {10, 20, 5, 6, 12, 30, 7, 17, 1, 2, 3, 4, 8,
                  9, 11, 13, 14, 15, 16, 18, 19, 21, 22, 23, };
    for (int i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
            keys[i]=rand()%10000;
        insert(tree, keys[i]);
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
    // ��ӡB��
    printf("B���ṹΪ:\n");
    printBTree(tree->root, 0);
    break;
    case 2:
    // ��������
    printf("������Ҫ����������\n");
    scanf("%d",&k);
    printf("%s\n", searchKey(tree->root,k) ? "���ҵ�" : "δ�ҵ�");
    break;
    case 3:
         printf("������Ҫ���������\n");
    scanf("%d",&k);
    insert(tree, k);
    printf("����%d֮��B����ͼ:\n", k);
    printBTree(tree->root, 0);
    break;
    case 4:
        printf("������Ҫɾ��������\n");
    scanf("%d",&k);
    // ɾ������
    deleteKey(tree->root,k);
    printf("ɾ��%d֮��B����ͼ:\n", k);
    printBTree(tree->root, 0);
    break;
        }
    }
    // �ͷ�B���ڴ�
    freeBTree(tree->root);
    free(tree);

    return 0;
}
