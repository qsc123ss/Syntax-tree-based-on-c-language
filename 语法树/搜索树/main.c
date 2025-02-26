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
// �����������ҵ��������е���Сֵ�ڵ�
BSTNode* findMinNode(BSTNode* root)
{
    BSTNode* current = root;
    while (current->left != NULL)
    {
        current = current->left;
    }
    return current;
}
BSTNode* deleteBST(BSTNode* root, int data)//һ���ڵ����û�нڵ����ֱ��ɾ���������ڵ����ұ�������Сֵ��
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
        // �ڵ�ֻ��һ���ӽڵ��û���ӽڵ�����
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
    printf(" ���%d:%d\n",num,root->data);
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

    // ����ڵ�
    for (int i = 0; i < n; i++)
    {
        a=rand()%10000;
        root = insertBST(root, a);
    }
    pri(root,1);
    while(1)
    {
        printf(" 0���˳�\n 1����ӡ\n 2������\n 3������\n 4��ɾ��\n");
        scanf("%d",&flag);
        if(flag==0)
            break;
        switch(flag)
        {
        case 1:
            // ���
            pri(root,1);
            break;
        case 2:
            // �����ڵ�
            printf("������Ҫ��Ѷ������\n");
            scanf("%d",&key);
            result = BSTsearch(root, key);
            if (result != NULL)
            {
                printf("%d������\n", key);
            }
            else
            {
                printf("%d��������\n", key);
            }
            break;
        case 3:
            printf("������Ҫ���������\n");
            scanf("%d",&key);
            root = insertBST(root, key);
            break;
        case 4:
            // ɾ���ڵ�
            printf("������Ҫɾ��������\n");
            scanf("%d",&key);
            root = deleteBST(root, key);
            break;
        }
    }


    // �ͷ��ڴ�
    freeBST(root);
    return 0;
}
