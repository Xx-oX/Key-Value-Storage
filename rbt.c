// C program for Red-Black Tree insertion

#include "rbt.h"

// left_child Rotation
void left_childRotate(Treenode **root, Treenode *x)
{
    if (!x || !x->right_child)
        return ;
    //y stored pointer of right_child child of x
    Treenode *y = x->right_child;

    //store y's left_child subtree's pointer as x's right_child child
    x->right_child = y->left_child;

    //update parent pointer of x's right_child
    if (x->right_child != NULL)
        x->right_child->parent = x;

    //update y's parent pointer
    y->parent = x->parent;

    // if x's parent is null make y as root of tree
    if (x->parent == NULL)
        (*root) = y;

    // store y at the place of x
    else if (x == x->parent->left_child)
        x->parent->left_child = y;
    else    x->parent->right_child = y;

    // make x as left_child child of y
    y->left_child = x;

    //update parent pointer of x
    x->parent = y;
}


// right_child Rotation (Similar to left_childRotate)
void right_childRotate(Treenode **root, Treenode *y)
{
    if (!y || !y->left_child)
        return ;
    Treenode *x = y->left_child;
    y->left_child = x->right_child;
    if (x->right_child != NULL)
        x->right_child->parent = y;
    x->parent =y->parent;
    if (x->parent == NULL)
        (*root) = x;
    else if (y == y->parent->left_child)
        y->parent->left_child = x;
    else y->parent->right_child = x;
    x->right_child = y;
    y->parent = x;
}

// Utility function to fixup the Red-Black tree after standard BST insertion
void insertFixUp(Treenode **root, Treenode *z)
{
    // iterate until z is not the root and z's parent color is red
    while (z != *root && z != (*root)->left_child && z != (*root)->right_child && z->parent->color == 'R')
    {
        Treenode *y;

        // Find uncle and store uncle in y
        if (z->parent && z->parent->parent && z->parent == z->parent->parent->left_child)
            y = z->parent->parent->right_child;
        else
            y = z->parent->parent->left_child;

        // If uncle is RED, do following
        // (i)  Change color of parent and uncle as BLACK
        // (ii) Change color of grandparent as RED
        // (iii) Move z to grandparent
        if (!y)
            z = z->parent->parent;
        else if (y->color == 'R')
        {
            y->color = 'B';
            z->parent->color = 'B';
            z->parent->parent->color = 'R';
            z = z->parent->parent;
        }

        // Uncle is BLACK, there are four cases (LL, LR, RL and RR)
        else
        {
            // left_child-left_child (LL) case, do following
            // (i)  Swap color of parent and grandparent
            // (ii) right_child Rotate Grandparent
            if (z->parent == z->parent->parent->left_child &&
                z == z->parent->left_child)
            {
                char ch = z->parent->color ;
                z->parent->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                right_childRotate(root,z->parent->parent);
            }

            // left_child-right_child (LR) case, do following
            // (i)  Swap color of current node  and grandparent
            // (ii) left_child Rotate Parent
            // (iii) right_child Rotate Grand Parent
            if (z->parent && z->parent->parent && z->parent == z->parent->parent->left_child &&
                z == z->parent->right_child)
            {
                char ch = z->color ;
                z->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                left_childRotate(root,z->parent);
                right_childRotate(root,z->parent->parent);
            }

            // right_child-right_child (RR) case, do following
            // (i)  Swap color of parent and grandparent
            // (ii) left_child Rotate Grandparent
            if (z->parent && z->parent->parent &&
                z->parent == z->parent->parent->right_child &&
                z == z->parent->right_child)
            {
                char ch = z->parent->color ;
                z->parent->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                left_childRotate(root,z->parent->parent);
            }

            // right_child-left_child (RL) case, do following
            // (i)  Swap color of current node  and grandparent
            // (ii) right_child Rotate Parent
            // (iii) left_child Rotate Grand Parent
            if (z->parent && z->parent->parent && z->parent == z->parent->parent->right_child &&
                z == z->parent->left_child)
            {
                char ch = z->color ;
                z->color = z->parent->parent->color;
                z->parent->parent->color = ch;
                right_childRotate(root,z->parent);
                left_childRotate(root,z->parent->parent);
            }
        }
    }
    (*root)->color = 'B'; //keep root always black
}

int insert_unique(Treenode **root, long long key, char *v)
{
    Treenode *z = find((*root), key);
    if(z == NULL){
        insert(root, key, v);
        return 0;
    }
    else{
        strncpy(z->value, v, VALUE_LEN);
        return 1;
    }
}

// Utility function to insert newly node in RedBlack tree
void insert(Treenode **root, long long key, char *v)
{
    // Allocate memory for new node
    Treenode *z = (Treenode*)malloc(sizeof(Treenode));
    z->key = key;
    strncpy(z->value, v, VALUE_LEN);
    z->left_child = z->right_child = z->parent = NULL;

     //if root is null make z as root
    if (*root == NULL)
    {
        z->color = 'B';
        (*root) = z;
    }
    else
    {
        Treenode *y = NULL;
        Treenode *x = (*root);

        // Follow standard BST insert steps to first insert the node
        while (x != NULL)
        {
            y = x;

            if (z->key < x->key)
                x = x->left_child;
            else
                x = x->right_child;
        }
    
        if (z->key > y->key)
            y->right_child = z;
        else
            y->left_child = z;

        z->parent = y;
        z->color = 'R';
        // call insertFixUp to fix reb-black tree's property if it
        // is voilated due to insertion.
        insertFixUp(root,z);
    }
}

Treenode* find(Treenode *root, long long k)
{
    if(root == NULL || root->key == k){
        return root;
    }
    if(root->value[0] == '\000'){
        return NULL;
    }
    if(k < root->key){
        return find(root->left_child, k);
    }
    else{
        return find(root->right_child, k);
    }
}

