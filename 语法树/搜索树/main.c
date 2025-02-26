#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define RED 0
#define BLACK 1
#define ORDER 5
#define MAX_KEYS (ORDER - 1)
#define MIDDLE (ORDER / 2)

typedef struct BSTNode
{
    int data;
    struct BSTNode *left, *right;
} BSTNode;
BSTNode* BSTcreateNode(int data)
{
    BSTNode* newNode = (BSTNode*)malloc(sizeof(BSTNode));
    newNode->data = data;
    newNode->left = newNode->right = NULL;
    return newNode;
}
BSTNode* insertBST(BSTNode* root, int data)
{
    if (root == NULL)
    {
        root = BSTcreateNode(data);
    }
    else if (data < root->data)
    {
        root->left = insertBST(root->left, data);
    }
    else if (data > root->data)
    {
        root->right = insertBST(root->right, data);
    }
    return root;
}
BSTNode* BSTsearch(BSTNode* root, int data)
{
    if (root == NULL || root->data == data)
    {
        return root;
    }
    if (data < root->data)
    {
        return BSTsearch(root->left, data);
    }
    else
    {
        return BSTsearch(root->right, data);
    }
}
// 辅助函数：找到右子树中的最小值节点
BSTNode* findMinNode(BSTNode* root)
{
    BSTNode* current = root;
    while (current->left != NULL)
    {
        current = current->left;
    }
    return current;
}
BSTNode* deleteBST(BSTNode* root, int data)//一个节点或者没有节点可以直接删除，两个节点用右边树的最小值。
{
    if (root == NULL) return root;

    if (data < root->data)
    {
        root->left = deleteBST(root->left, data);
    }
    else if (data > root->data)
    {
        root->right = deleteBST(root->right, data);
    }
    else
    {
        // 节点只有一个子节点或没有子节点的情况
        if (root->left == NULL)
        {
            BSTNode *temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL)
        {
            BSTNode *temp = root->left;
            free(root);
            return temp;
        }
        BSTNode* temp = findMinNode(root->right);
        root->data = temp->data;
        root->right = deleteBST(root->right, temp->data);
    }
    return root;
}
void pri(BSTNode* root,int num)
{
    if(root==NULL)
        return 0;
    printf(" 编号%d:%d\n",num,root->data);
    pri(root->left,2*num);
    pri(root->right,2*num+1);
}
void freeBST(BSTNode* root)
{
    if (root != NULL)
    {
        freeBST(root->left);
        freeBST(root->right);
        free(root);
    }
}
int main()
{
    srand(time(NULL));
    int flag,key;
    BSTNode* root=NULL;
    BSTNode* result=NULL;
    int a;
    int n =15;

    // 插入节点
    for (int i = 0; i < n; i++)
    {
        a=rand()%10000;
        root = insertBST(root, a);
    }
    pri(root,1);
    while(1)
    {
        printf(" 0）退出\n 1）打印\n 2）搜索\n 3）插入\n 4）删除\n");
        scanf("%d",&flag);
        if(flag==0)
            break;
        switch(flag)
        {
        case 1:
            // 输出
            pri(root,1);
            break;
        case 2:
            // 搜索节点
            printf("请输入要查讯的数：\n");
            scanf("%d",&key);
            result = BSTsearch(root, key);
            if (result != NULL)
            {
                printf("%d在树里\n", key);
            }
            else
            {
                printf("%d不在树里\n", key);
            }
            break;
        case 3:
            printf("请输入要插入的数：\n");
            scanf("%d",&key);
            root = insertBST(root, key);
            break;
        case 4:
            // 删除节点
            printf("请输入要删除的数：\n");
            scanf("%d",&key);
            root = deleteBST(root, key);
            break;
        }
    }


    // 释放内存
    freeBST(root);
    return 0;
}
