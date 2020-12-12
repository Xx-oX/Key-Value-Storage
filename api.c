#include "api.h"

/*global variables*/
FILE *ptr_out;
Listnode *tbl_loaded;
int is_first_line = 0;
int MAX_BUFFER_NUM;

status_t save()
{   
    status_t ret = OK;
    Listnode *curr = tbl_loaded;
    while(curr != NULL){
        flush_buffer(curr);
        curr = curr->next;
    }
    return ret;
}

status_t show_current_list()
{
#if(DEBUG_MODE == 1)
    printf("Current list:\n");
#endif    
    int cnt = 0;
    Listnode *curr = tbl_loaded;
    while(curr != NULL){
#if(DEBUG_MODE == 1)
    printf("%d:%s\n", cnt++, curr->path);
#endif
        flush_buffer(curr);
        curr = curr->next;
    }
    return OK;
}

status_t init_db(FILE *p, unsigned long mem_size)
{
    status_t ret = OK;

#if(DEBUG_MODE == 1)
    printf("[DEBUG]Initializing the database! \n");
#endif

    ret = set_file_pointer(p);
    
    if(ret == OK){
        tbl_loaded = NULL;
        MAX_BUFFER_NUM = mem_size / (8192 * 168);
        printf("[INIT]Set MAX_BUFFER_NUM to %d\n", MAX_BUFFER_NUM);
    }
    return ret;
}

status_t set_file_pointer(FILE *p)
{
    /*[ATTN]File should be opened*/
    status_t ret = OK;
    if(p == NULL) ret = ERR_MEM;
    if(ret == OK){
        ptr_out = p;
    }
    return ret;
}

status_t free_buffer(Listnode *node)
{
    return recur_free(node->root);
}

status_t recur_free(Treenode *root)
{
    status_t ret = OK;
    if (root == NULL)
        return OK;

    if(ret == OK){
        ret = recur_free(root->left_child);
        ret = recur_free(root->right_child);
        if(root->left_child != NULL) root->left_child = NULL;
        if(root->right_child != NULL) root->right_child = NULL;
        free(root);
    }
    return ret;
}

status_t flush_buffer(Listnode *node)
{
    if(node == NULL) return OK;

    if(vfork() == 0)
    {
        execlp("./mkfile.sh", "./mkfile.sh", node->dir, node->path, NULL);
    }

    FILE *ptr_f = fopen(node->path, "w");
    if(ptr_f == NULL) return ERR_FILE;

    status_t ret = recur_flush(ptr_f, node->root);

    fclose(ptr_f);
    return ret;
}

status_t recur_flush(FILE* ptr_f, Treenode *root)
{
    status_t ret = OK;
    if (root == NULL)
        return OK;

    if(ret == OK){

        ret = recur_flush(ptr_f, root->left_child);

        fprintf(ptr_f, "%lld:%s\n", root->key, root->value);
#if(DEBUG_MODE == 1)
        printf("%lld:%s\n", root->key, root->value);
#endif

        ret = recur_flush(ptr_f, root->right_child);
    }
    return ret;
}

status_t load_buffer(Listnode *node)
{
    /*[ATTN]Memory of node should be allocated*/
    status_t ret = OK;

    if(vfork() == 0)
    {
        execlp("./mkfile.sh", "./mkfile.sh", node->dir, node->path, NULL);
    }

    FILE *ptr_f = fopen(node->path, "r");
    if(ptr_f == NULL) return ERR_FILE;

    long long k;
    char v[VALUE_LEN];
    while(fscanf(ptr_f, "%lld:%s\n", &k, v) != EOF){
        insert_unique(&(node->root), k, v);
        node->size++;
    }

    fclose(ptr_f);
    return ret;
}

Listnode *find_tbl(char *p)
{
    Listnode *curr = tbl_loaded;
    while(curr != NULL){
        if(strcmp(curr->path, p) == 0){
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

status_t check_list_size(int *ret)
{
    Listnode *curr = tbl_loaded;
    int cnt = 0;
    while(curr != NULL){
        cnt++;
        curr = curr->next;
    }
    *ret = cnt;
}

status_t flush_list()
{
    /*pop the head node*/
    Listnode *curr = tbl_loaded;
    tbl_loaded = tbl_loaded->next;
    flush_buffer(curr);
    curr->next = NULL;
    free(curr);
    return OK;
}

status_t load_list(Listnode* tbl_in)
{
    // need to ensure that len(tbl_loaded<MAX_BUFFER_NUM)
    Listnode *curr = tbl_loaded;
    Listnode *prev = NULL;
    while(curr != NULL){
        prev = curr;
        curr = curr->next;        
    }
    if(prev == NULL){
        tbl_loaded = tbl_in;
    }else{
        prev->next = tbl_in;
    }
    return OK;
}

status_t api_put(long long k, char *v)
{
    status_t ret = OK;

    if(ptr_out == NULL){
        ret = ERR_FILE;
    }

#if(DEBUG_MODE == 1)
    printf("[DEBUG]put {%lld : %s} \n", k, v);
#endif

    if(v == NULL) ret = ERR_MEM;

    if(ret == OK){
        long long num = k;
        long long first = num/FIRST_MASK_SIZE;
        num %= FIRST_MASK_SIZE;
        long long second = num/SECOND_MASK_SIZE;

	    char dir[MAX_PATH_SIZE];
        char path[MAX_PATH_SIZE];
        sprintf(dir, "./storage/%lld/", first);
        sprintf(path, "%s%lld.tbl", dir, second);

        Listnode *node = find_tbl(path);

        if(node != NULL){
            // tbl is loaded
            if(insert_unique(&node->root, k, v) == 0)
                node->size++;
        }
        else{
            int loaded_num;
            check_list_size(&loaded_num);
            if(loaded_num == MAX_BUFFER_NUM){
                flush_list();
            }

            Listnode *new_tbl = malloc(sizeof(Listnode));
            strncpy(new_tbl->dir, dir, MAX_PATH_SIZE);
            strncpy(new_tbl->path, path, MAX_PATH_SIZE);
            new_tbl->size = 0;
            new_tbl->root = NULL;
            new_tbl->next = NULL;

            load_list(new_tbl);
            load_buffer(new_tbl);
            
            if(insert_unique(&new_tbl->root, k, v) == 0)
                new_tbl->size++;
        }   
    }
    return ret;
}

status_t api_get(long long k)
{
    status_t ret = OK;

    if(ptr_out == NULL){
        ret = ERR_FILE;
    }

#if(DEBUG_MODE == 1)
    printf("[DEBUG]get {%lld} \n", k);
#endif

    if(ret == OK){
        long long num = k;
        long long first = num/FIRST_MASK_SIZE;
        num %= FIRST_MASK_SIZE;
        long long second = num/SECOND_MASK_SIZE;

	    char dir[MAX_PATH_SIZE];
        char path[MAX_PATH_SIZE];
        sprintf(dir, "./storage/%lld/", first);
        sprintf(path, "%s%lld.tbl", dir, second);

        Listnode *node = find_tbl(path);
        Treenode *result;

        if(node != NULL){
            result = find(node->root, k);
        }
        else{
            int loaded_num;
            check_list_size(&loaded_num);
            if(loaded_num == MAX_BUFFER_NUM){
                flush_list();
            }

            Listnode *new_tbl = malloc(sizeof(Listnode));
            strncpy(new_tbl->dir, dir, MAX_PATH_SIZE);
            strncpy(new_tbl->path, path, MAX_PATH_SIZE);
            new_tbl->size = 0;
            new_tbl->root = NULL;
            new_tbl->next = NULL;

            load_list(new_tbl);
            load_buffer(new_tbl);
            result = find(new_tbl->root, k);                        
        }

        // output
        if(is_first_line == 0){
            is_first_line = 1;
        }
        else{
            fwrite("\n", 1, 1, ptr_out);
        }
        if(result == NULL){
            fwrite("EMPTY", sizeof(char) * 5, 1, ptr_out);
        }
        else{
            fwrite(result->value, sizeof(char)*VALUE_LEN, 1, ptr_out);
        }
    }
    return ret;
}

status_t api_scan(long long a, long long b)
{
    status_t ret = OK;

    if(ptr_out == NULL){
        ret = ERR_FILE;
    }

#if(DEBUG_MODE == 1)
    printf("[DEBUG]scan {%lld ~ %lld} \n", a, b);
#endif

    if(ret == OK){
        long long num, first, second, old_first, old_second;
        Listnode *node;
        Treenode *result;
        for(long long i=a; i<=b; ++i){
            num = i;
            first = num/FIRST_MASK_SIZE;
            num %= FIRST_MASK_SIZE;
            second = num/SECOND_MASK_SIZE;
            
            if(i == a){
                // first time    
                old_first = first;
                old_second = second;
                char dir[MAX_PATH_SIZE];
                char path[MAX_PATH_SIZE];
                sprintf(dir, "./storage/%lld/", first);
                sprintf(path, "%s%lld.tbl", dir, second);
                node = find_tbl(path);

                if(node != NULL){
                    result = find(node->root, i);
                }
                else{
                    int loaded_num;
                    check_list_size(&loaded_num);
                    if(loaded_num == MAX_BUFFER_NUM){
                        flush_list();
                    }

                    Listnode *new_tbl = malloc(sizeof(Listnode));
                    strncpy(new_tbl->dir, dir, MAX_PATH_SIZE);
                    strncpy(new_tbl->path, path, MAX_PATH_SIZE);
                    new_tbl->size = 0;
                    new_tbl->root = NULL;
                    new_tbl->next = NULL;

                    load_list(new_tbl);
                    load_buffer(new_tbl);
                    node = new_tbl;
                    result = find(new_tbl->root, i);                        
                }
            }
            else if(first == old_first && second == old_second){
                if(node != NULL)
                    result = find(node->root, i);
                else
                    result = NULL;
            }
            else{
                old_first = first;
                old_second = second;
                char dir[MAX_PATH_SIZE];
                char path[MAX_PATH_SIZE];
                sprintf(dir, "./storage/%lld/", first);
                sprintf(path, "%s%lld.tbl", dir, second);
                node = find_tbl(path);

                if(node != NULL){
                    result = find(node->root, i);
                }
                else{
                    // node not found
                    if(access(path, F_OK) == 0){
                        // file exists => need to load
                        int loaded_num;
                        check_list_size(&loaded_num);
                        if(loaded_num == MAX_BUFFER_NUM){
                            flush_list();
                        }

                        Listnode *new_tbl = malloc(sizeof(Listnode));
                        strncpy(new_tbl->dir, dir, MAX_PATH_SIZE);
                        strncpy(new_tbl->path, path, MAX_PATH_SIZE);
                        new_tbl->size = 0;
                        new_tbl->root = NULL;
                        new_tbl->next = NULL;

                        load_list(new_tbl);
                        load_buffer(new_tbl);
                        node = new_tbl;
                        result = find(new_tbl->root, i);
                    }
                    else{
                        // file don't exists => EMPTY
                        node = NULL;
                        result = NULL;
                    }                       
                }
            }
            // output
            if(is_first_line == 0){
                is_first_line = 1;
            }
            else{
                fwrite("\n", 1, 1, ptr_out);
            }
            if(result == NULL){
                fwrite("EMPTY", sizeof(char) * 5, 1, ptr_out);
            }
            else{
                fwrite(result->value, sizeof(char)*VALUE_LEN, 1, ptr_out);
            }
        }
    }
    return ret;
}