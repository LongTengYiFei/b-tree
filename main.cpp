#include "btreeRAM.h"
#include <thread>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, char **argv){
    int numData = 0;
    int n_threads = 1;
    char *run_path = "astl";
    char *load_path = "ltsa";
    int c;
    while ((c = getopt(argc, argv, "n:w:t:i:l:")) != -1) {
        switch (c) {
            case 'n':
                numData = atoi(optarg);
                break;
            case 't':
                n_threads = atoi(optarg);
                break;
            case 'i':
                run_path = optarg;
            case 'l':
                load_path = optarg;
            default:
                break;
        }
    }


    btree *bt;
    bt = new btree(4);
    const char* aa = "hello";
    const char* bb = "world";
    const char* cc = "cyf";

    bt->Insert(1, cc);
    bt->Insert(2, bb);
    bt->Insert(3, aa);

    char* ans1 = bt->Get(1);
    char* ans2 = bt->Get(4);
    if(ans1)
        printf("%s\n", ans1);
    if(ans2)
        printf("%s\n", ans2);
    
    return 0;
}