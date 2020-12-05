#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "types.h"
#include "api.h"

/*global variables*/
FILE *ptr_fin;
FILE *ptr_fout;

/*function declaration*/
void read_input();
void debug(status_t);

void read_input()
{
    while(1){
        char buffer[MAX_STR_SIZE];
        if(fgets(buffer, MAX_STR_SIZE, ptr_fin) == NULL)
            break;

        char *instr = strtok(buffer, " ");
        if(strcmp(instr, "PUT") == 0){
            char *str_key = strtok(NULL, " ");
            char *str_value = strtok(NULL, " ");
            long long ll_key = atoll(str_key);

            debug(api_put(ll_key, str_value));

        }
        else if(strcmp(instr, "GET") == 0){
            char *str_key = strtok(NULL, " ");
            long long ll_key = atoll(str_key);

            debug(api_get(ll_key));
            
        }
        else if(strcmp(instr, "SCAN") == 0){
            char *str_key1 = strtok(NULL, " ");
            char *str_key2 = strtok(NULL, " ");
            long long ll_key1 = atoll(str_key1);
            long long ll_key2 = atoll(str_key2);

            debug(api_scan(ll_key1, ll_key2));

        }
        else{
            printf("[ERR]Input error!!!\n");
            exit(-1);
        }
    }
    fclose(ptr_fin);
}

void debug(status_t ret)
{
#if(DEBUG_MODE == 1)
    switch (ret)
    {
    case OK:
        printf("OK\n");
        break;

    case ERR:
        printf("ERR\n");
        break;

    case ERR_MEM:
        printf("ERR_MEM\n");
        break;

    case ERR_FILE:
        printf("ERR_FILE\n");
        break;
    
    default:
        break;
    }
#endif
}

int main(int argc, char* argv[])
{
    double time_start, time_mid, time_end;
    time_start = clock();
    /*deal with input/output*/
    if(argc < 2){
        printf("[ERR]Input path is needed!!!\n");
        exit(-1);
    }
    char str_tmp[strlen(argv[1])];
    strcpy(str_tmp, argv[1]);
    
    /*set output path*/
    char *token = strtok(str_tmp, "/");
    char *file_name;
    while(token != NULL){
        file_name = token;
        token = strtok(NULL, "/");
    }
    file_name = strtok(file_name, ".");
    int len_output_path = 2 + strlen(file_name) + 7 + 1;
    char output_path[len_output_path];
    sprintf(output_path, "./%s.output", file_name);
    
    if((ptr_fin = fopen(argv[1], "r")) == NULL){
        printf("[ERR]Fail to open inputfile!!! \n");
        exit(-1);
    } 

    if((ptr_fout = fopen(output_path, "w")) == NULL){
        printf("[ERR]Fail to create outputfile!!! \n");
        exit(-1);
    }

    //shell script that creates directory "./storage"
    if(vfork() == 0)
    {
        execlp("./mkdir.sh", "./mkdir.sh", "./storage", NULL);
    }

    debug(init_db(ptr_fout));

    read_input();

    debug(save());

    fclose(ptr_fout);
    
    time_end = clock();
    printf("Total time:%lf sec \n", (time_end - time_start)/CLOCKS_PER_SEC);
    return 0;
}