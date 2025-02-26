#include <stdio.h>
#include <stdlib.h>
typedef struct AVLNode
{
    int key;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;
int height(AVLNode *N)
{
    if (N == NULL)
        return 0;
    return N->height;
}

int getBalance(AVLNode *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}
AVLNode* rightRotate(AVLNode* y)
{
    AVLNode* x = y->left;
    AVLNode* temp = x->right;
    x->right = y;
    y->left = temp;
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;
    return x;
}

AVLNode* leftRotate(AVLNode* x)
{
    AVLNode* y = x->right;
    AVLNode* temp = y->left;
    y->left = x;
    x->right = temp;
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;
    return y;
}
int max(int a, int b)
{
    return (a > b) ? a : b;
}
AVLNode* insert(AVLNode* node, int key)
{
    if (node == NULL)
        {
        AVLNode* p=(AVLNode*)malloc(sizeof(AVLNode));
        p->key=key;
        p->left=NULL;p->right=NULL;
        p->height=0;
        return p;
        }

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else
        return node;
    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);
    if (balance > 1 && key < node->left->key)//LL
        return rightRotate(node);
    if (balance < -1 && key > node->right->key)//RR
        return leftRotate(node);
    if (balance > 1 && key > node->left->key)//LR
    {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && key < node->right->key)//RL
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}
AVLNode* minValueNode(AVLNode* node)
{
    AVLNode* current = node;
    while (current && current->left != NULL)
        current = current->left;
    return current;
}

AVLNode* deleteNode(AVLNode* root, int key)
{
    if (root == NULL)
        return root;

    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else
    {
        if ((root->left == NULL) || (root->right == NULL))
        {
            AVLNode *temp = root->left ? root->left : root->right;
            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else
                *root = *temp;

            free(temp);
        }
        else
        {
            AVLNode* temp = minValueNode(root->right);
            root->key = temp->key;
            root->right = deleteNode(root->right, temp->key);
        }
    }
    if (root == NULL)
        return root;
    root->height = max(height(root->left), height(root->right)) + 1;
    int balance = getBalance(root);
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);
    if (balance > 1 && getBalance(root->left) < 0)
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);
    if (balance < -1 && getBalance(root->right) > 0)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}
AVLNode* AVLsearch(AVLNode* root, int key)
{
    if (root == NULL || root->key == key)
        return root;
    if (root->key < key)
        return AVLsearch(root->right, key);
    return AVLsearch(root->left, key);
}
void pri(AVLNode* root,int num)
{
    if(root==NULL)
        return;
    printf(" 编号%d:%d\n",num,root->key);
    pri(root->left,2*num);
    pri(root->right,2*num+1);
}
int main()
{
    srand(time(NULL));
    int a,flag;
    AVLNode *root = NULL;
    AVLNode *found;
    // 插入一些节点
    for(int i=0; i<15; i++)
    {
        a=rand()%10000;
        root = insert(root, a);
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
            pri(root,1);
            break;
        case 2:
            // 搜索节点
            printf("请输入要查讯的数：\n");
            scanf("%d",&a);
            found = AVLsearch(root, a);
            if (found != NULL)
                printf("树中有%d\n", found->key);
            else
                printf("没找到\n");
            break;
        case 3:
            printf("请输入要插入的数：\n");
            scanf("%d",&a);
            root = insert(root, a);
            break;
        case 4:
            printf("请输入要删除的数：\n");
            scanf("%d",&a);
            // 删除节点
            root = deleteNode(root,a);
            break;
        }
    }


    return 0;
}
