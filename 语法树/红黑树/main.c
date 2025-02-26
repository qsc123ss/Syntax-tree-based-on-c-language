#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// 定义红黑树节点的颜色
#define RED 0
#define BLACK 1

// 定义红黑树节点结构体
typedef struct RBNode
{
    int data;
    int color;
    struct RBNode* left;
    struct RBNode* right;
    struct RBNode* parent;
} RBNode;

// 创建新节点
RBNode* createNode(int data)
{
    RBNode* newNode = (RBNode*)malloc(sizeof(RBNode));
    newNode->data = data;
    newNode->color = RED;
    newNode->left=NULL;
    newNode->right = NULL;
    newNode->parent = NULL;
    return newNode;
}

// 左旋转
void leftRotate(RBNode** root, RBNode* x)
{
    RBNode* y = x->right;
    x->right = y->left;
    if (y->left != NULL)
    {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == NULL)
    {
        *root = y;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

// 右旋转
void rightRotate(RBNode** root, RBNode* y)
{
    RBNode* x = y->left;
    y->left = x->right;
    if (x->right != NULL)
    {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == NULL)
    {
        *root = x;
    }
    else if (y == y->parent->left)
    {
        y->parent->left = x;
    }
    else
    {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
}

// 插入修复
void insertFixup(RBNode** root, RBNode* z)
{
    while (z->parent != NULL && z->parent->color == RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            RBNode* y = z->parent->parent->right;
            if (y != NULL && y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    z = z->parent;
                    leftRotate(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(root, z->parent->parent);
            }
        }
        else
        {
            RBNode* y = z->parent->parent->left;
            if (y != NULL && y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    rightRotate(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(root, z->parent->parent);
            }
        }
    }
    (*root)->color = BLACK;
}

// 插入节点到红黑树中
void insert(RBNode** root, int data)
{
    RBNode* z = createNode(data);
    RBNode* y = NULL;
    RBNode* x = *root;

    while (x != NULL)
    {
        y = x;
        if (z->data < x->data)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == NULL)
    {
        *root = z;
    }
    else if (z->data < y->data)
    {
        y->left = z;
    }
    else
    {
        y->right = z;
    }
    z->color = RED;
    insertFixup(root, z);
}

// 查找节点
RBNode* search(RBNode* root, int data)
{
    if (root == NULL || root->data == data)
    {
        return root;
    }
    if (data < root->data)
    {
        return search(root->left, data);
    }
    return search(root->right, data);
}

// 获取最小值节点
RBNode* minValueNode(RBNode* node)
{
    RBNode* current = node;
    while (current->left != NULL)
    {
        current = current->left;
    }
    return current;
}

// 删除修复
void deleteFixup(RBNode** root, RBNode* x)
{
    while (x != *root && (x == NULL || x->color == BLACK))
    {
        if (x == x->parent->left)
        {
            RBNode* w = x->parent->right;
            if (w->color == RED)
            {
                w->color = BLACK;
                x->parent->color = RED;
                leftRotate(root, x->parent);
                w = x->parent->right;
            }
            if ((w->left == NULL || w->left->color == BLACK) &&
                    (w->right == NULL || w->right->color == BLACK))
            {
                w->color = RED;
                x = x->parent;
            }
            else
            {
                if (w->right == NULL || w->right->color == BLACK)
                {
                    if (w->left != NULL)
                    {
                        w->left->color = BLACK;
                    }
                    w->color = RED;
                    rightRotate(root, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->right != NULL)
                {
                    w->right->color = BLACK;
                }
                leftRotate(root, x->parent);
                x = *root;
            }
        }
        else
        {
            RBNode* w = x->parent->left;
            if (w->color == RED)
            {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(root, x->parent);
                w = x->parent->left;
            }
            if ((w->right == NULL || w->right->color == BLACK) &&
                    (w->left == NULL || w->left->color == BLACK))
            {
                w->color = RED;
                x = x->parent;
            }
            else
            {
                if (w->left == NULL || w->left->color == BLACK)
                {
                    if (w->right != NULL)
                    {
                        w->right->color = BLACK;
                    }
                    w->color = RED;
                    leftRotate(root, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                if (w->left != NULL)
                {
                    w->left->color = BLACK;
                }
                rightRotate(root, x->parent);
                x = *root;
            }
        }
    }
    if (x != NULL)
    {
        x->color = BLACK;
    }
}

// 删除节点
void deleteNode(RBNode** root, int data)
{
    RBNode* z = search(*root, data);
    if (z == NULL)
    {
        return;
    }

    RBNode* y = z;
    RBNode* x;
    int yOriginalColor = y->color;

    if (z->left == NULL)
    {
        x = z->right;
        if (z->parent == NULL)
        {
            *root = x;
        }
        else if (z == z->parent->left)
        {
            z->parent->left = x;
        }
        else
        {
            z->parent->right = x;
        }
        if (x != NULL)
        {
            x->parent = z->parent;
        }
    }
    else if (z->right == NULL)
    {
        x = z->left;
        if (z->parent == NULL)
        {
            *root = x;
        }
        else if (z == z->parent->left)
        {
            z->parent->left = x;
        }
        else
        {
            z->parent->right = x;
        }
        if (x != NULL)
        {
            x->parent = z->parent;
        }
    }
    else
    {
        y = minValueNode(z->right);
        yOriginalColor = y->color;
        x = y->right;
        if (y->parent == z)
        {
            if (x != NULL)
            {
                x->parent = y;
            }
        }
        else
        {
            if (y->parent != NULL)
            {
                if (y == y->parent->left)
                {
                    y->parent->left = x;
                }
                else
                {
                    y->parent->right = x;
                }
            }
            if (x != NULL)
            {
                x->parent = y->parent;
            }
            y->right = z->right;
            if (y->right != NULL)
            {
                y->right->parent = y;
            }
        }
        if (z->parent == NULL)
        {
            *root = y;
        }
        else if (z == z->parent->left)
        {
            z->parent->left = y;
        }
        else
        {
            z->parent->right = y;
        }
        y->parent = z->parent;
        y->color = z->color;
        y->left = z->left;
        if (y->left != NULL)
        {
            y->left->parent = y;
        }
    }
    free(z);
    if (yOriginalColor == BLACK)
    {
        deleteFixup(root, x);
    }
}

// 打印
void pri(RBNode* root,int num)
{
    if(root==NULL)
        return;
    if(root->color==0)
        printf(" 编号%d:%d 颜色红\n",num,root->data);
    if(root->color==1)
        printf(" 编号%d:%d 颜色黑\n",num,root->data);
    pri(root->left,2*num);
    pri(root->right,2*num+1);
}
// 主函数
int main()
{
    srand(time(NULL));
    RBNode* found=NULL;
    RBNode* root = NULL;
    int a,flag;
    for (int i = 0; i < 15; i++)
    {
        a = rand() % 10000;
        printf("%d\n",a);
        insert(&root, a);
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
            // 打印树的层次遍历结构
            pri(root,1);
            break;
        case 2:
            printf("请输入要查讯的数：\n");
            scanf("%d",&a);
            found = search(root,a);
            if (found != NULL)
            {
                printf("找到节点 %d\n",a);
            }
            else
            {
                printf("未找到节点 %d\n",a);
            }
            break;
        case 3:
            printf("请输入要插入的数：\n");
            scanf("%d",&a);
            insert(&root, a);
            break;
        case 4:
            // 删除一个节点
            printf("请输入要删除的数：\n");
            scanf("%d",&a);
            printf("删除节点 %d:\n", a);
            deleteNode(&root,a);
            pri(root,1);
            break;
        }
    }
    return 0;
}


