#include <sys/sysinfo.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unordered_map>


#define MAX_STR_SIZE 256

std::unordered_map<long long, std::string> cache;
FILE *ptr_fin;
FILE *ptr_fout;
bool is_first = false;

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
            std::string value(str_value);
            auto ret = cache.find(ll_key);
            if(ret != cache.end()){
                cache[ll_key] = value;
            }
            else{
                cache.emplace(ll_key, value);
            }
        }
        else if(strcmp(instr, "GET") == 0){
            char *str_key = strtok(NULL, " ");
            long long ll_key = atoll(str_key);
            
            auto result = cache.find(ll_key);
            if(!is_first) is_first = true;
            else{
                fwrite("\n", 1, 1, ptr_fout);
            }
            if(result != cache.end()){
                fwrite(result->second.c_str(), 128, 1, ptr_fout);
            } 
            else{
                fwrite("EMPTY", 5, 1, ptr_fout);
            } 
            
        }
        else if(strcmp(instr, "SCAN") == 0){
            char *str_key1 = strtok(NULL, " ");
            char *str_key2 = strtok(NULL, " ");
            long long ll_key1 = atoll(str_key1);
            long long ll_key2 = atoll(str_key2);

            for(auto i=ll_key1; i<=ll_key2; ++i){
                auto result = cache.find(i);
                if(!is_first) is_first = true;
                else{
                    fwrite("\n", 1, 1, ptr_fout);
                }
                if(result != cache.end()){
                    fwrite(result->second.c_str(), 128, 1, ptr_fout);
                } 
                else{
                    fwrite("EMPTY", 5, 1, ptr_fout);
                } 
            }

        }
        else{
            printf("[ERR]Input error!!!\n");
            exit(-1);
        }
    }
    fclose(ptr_fin);
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

    read_input();

    time_end = clock();
    printf("Total time:%lf sec \n", (time_end - time_start)/CLOCKS_PER_SEC);
    return 0;
}
