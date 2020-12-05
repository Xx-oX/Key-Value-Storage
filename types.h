#ifndef TYPES_H
#define TYPES_H

/*1 for debug mode*/
#define DEBUG_MODE 0
/*length of value*/
#define VALUE_LEN 128
/*max length of char[]*/
#define MAX_STR_SIZE 256
/*max length of char *path*/
#define MAX_PATH_SIZE 64

typedef enum{
    OK,
    ERR,
    ERR_MEM,
    ERR_FILE
} status_t; 

typedef struct TREENODE{
    struct TREENODE *left_child;
    struct TREENODE *right_child;
    struct TREENODE *parent;
    char color;
    long long key;
    char value[VALUE_LEN];    
} Treenode;                 // 168 bytes 

typedef struct LISTNODE{
    char dir[MAX_PATH_SIZE];
    char path[MAX_PATH_SIZE];
    int size;
    Treenode *root;
    struct LISTNODE *next;
} Listnode;                 // 152 bytes

#endif