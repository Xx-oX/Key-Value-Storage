#ifndef API_H
#define APT_H

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "rbt.h"

#define FIRST_MASK_SIZE 8589934592      // 2^33
#define SECOND_MASK_SIZE 8192           // 2^13
/*63 = 30 + 20 + 13*/ 

// #define MAX_BUFFER_NUM 4096             // 168 bytes * 2^13 * 2^12 = 5376 Mb in total 
 
status_t save();
status_t show_current_list();
status_t init_db(FILE *, unsigned long);
status_t set_file_pointer(FILE*);

status_t free_buffer(Listnode*);
status_t recur_free(Treenode*);
status_t flush_buffer(Listnode*);
status_t recur_flush(FILE*, Treenode*);
status_t load_buffer(Listnode*);

// queue of loaded table
Listnode* find_tbl(char*);
status_t check_list_size(int*);
status_t flush_list();
status_t load_list(Listnode*);

status_t api_put(long long, char*);
status_t api_get(long long);
status_t api_scan(long long, long long);

#endif