#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <string>
#include <functional>
#include <algorithm>

using namespace std;

std::string random_string(string::size_type length)
{
    static auto& chrs = "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{std::random_device{}()};
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string s;

    s.reserve(length);

    while(length--)
        s += chrs[pick(rg)];

    return s;
}

int main(int argc, char *argv[])
{
    double time_start, time_mid, time_end;
    time_start = clock();

    std::mt19937 rg( time(NULL) );
    std::uniform_int_distribution<long long> r_key;
    std::uniform_int_distribution<int> r_instr(0,100);
    std::uniform_int_distribution<int> r_scan_range(0,1000);

    long long size = 0;
    if(argc > 1){
        size = atoll(argv[1]);
    }
    printf(">Generate a test file with %lld amount of instructions. \n", size);

    ofstream f_out;
    f_out.open("large_test.input", ios::out | ios::trunc);

    vector<long long> used;

    for(long long i=0; i<size; ++i){
        
        int a = r_instr(rg);

        if (a < 1) {
            string instr = "SCAN ";
            long long key1 = r_key(rg);
            long long key2 = key1 + r_scan_range(rg);
            f_out << instr << key1 << " " << key2 << "\n";
        }
        else if(a < 15) {
            string instr = "GET ";
            int x = r_instr(rg);
            long long key;
            if(!used.empty() && x > 10){
                std::uniform_int_distribution<int> r_index(0,used.size()-1);
                key = used.at(r_index(rg));
            }
            else{
                key = r_key(rg);
                used.push_back(key);
            }   
            f_out << instr << key << "\n";
        }
        else {
            string instr = "PUT ";
            int x = r_instr(rg);
            long long key;
            if(!used.empty() && x > 10){
                std::uniform_int_distribution<int> r_index(0,used.size()-1);
                key = used.at(r_index(rg));
            }
            else{
                key = r_key(rg);
                used.push_back(key);
            }   

            string value = random_string(128);
            f_out << instr << key << " " << value << "\n";
        }
    }
    
    f_out.close();

    time_end = clock();
    printf("Total time:%lf sec \n", (time_end - time_start)/CLOCKS_PER_SEC);
    return 0;
}
