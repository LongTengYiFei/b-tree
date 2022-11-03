#include <cassert>
#include <climits>
#include <fstream>
#include <future>
#include <iostream>
#include <math.h>
#include <mutex>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <vector>

using namespace std;

enum NODE_TYPE{
    internal_node,
    leaf,
};

class node;

class btree {
    private:
        int m;
        node* root_node;
        int height;
        pthread_rwlock_t rwlock;

    public:
        btree();
        btree(int);
        char* Get(uint64_t);
        void Insert(uint64_t, const char *);
        void Update(uint64_t, const char *);
    friend class node;
};

class node{
    private:
        int m;
        int key_count;
        NODE_TYPE node_type;
        node* next_leaf_node;
        node* parent;
        btree* bt;

        node** children;
        uint64_t* keys;
        char** values;

        node(){
            this->m = 0;
            this->key_count = 0;
            this->node_type = leaf;
            this->next_leaf_node = nullptr;
            this->parent = nullptr;
            this->bt = nullptr;

            this->children = new node* [this->m];
            for(int i=0; i<=this->m-1; i++)
                this->children[i] = nullptr;
            
            this->keys = (uint64_t*)malloc(sizeof(uint64_t) * (this->m-1));
            this->values = (char**)malloc(sizeof(char*) * (this->m-1));
            for(int i=0; i<=m-2; i++){
                keys[i] = 0;
                values[i] = nullptr;
            }
        }

        char* get(uint64_t key){
            if(this->node_type == leaf)
                return get_leaf_node(key);
            else
                return get_internal_node(key);
        }

        char* get_internal_node(uint64_t key){
            for(int i=0; i<=key_count-1; i++){
                if(keys[i] == key){
                    return children[i+1]->get(key);
                }else if(keys[i] > key){
                    ;
                }else if(keys[i] < key){
                    return children[i]->get(key);
                }
            }
            return children[key_count]->get(key);
        }

        char* get_leaf_node(uint64_t key){
            if(keys == nullptr)
                printf("keys is null, should not reach here\n");
            for(int i=0; i<=key_count-1; i++){     
                if(keys[i] == key){
                    if(values[i] == nullptr)
                        printf("values %d is null, should not reach here\n", i);
                    return values[i];
                }else if(keys[i] > key){
                    ;
                }else if(keys[i] < key){
                    return nullptr;
                }
            }
            return nullptr;
        }

        //默认插入的是以\0结尾的字符串
        void insert(uint64_t key, const char* value){
            if(this->node_type == leaf)
                //根节点就是叶子节点
                insert_leaf_node(key, value);
            else{
                //寻找应当插入的叶子节点

            }
        }

        //这个函数只能被分裂触发
        void insert_internal_node(uint64_t key, const char* value, node* left, node* right){
            key_count++;
            if(key_count <= m-1){

            }else{

            }
        }

        //尝试插入只能是叶子节点
        void insert_leaf_node(uint64_t key, const char* value){
            key_count++;
            if(key_count <= m-1){
                //无需分裂
                keys[key_count-1] = key;
                values[key_count-1] = (char*)malloc(sizeof(char) * (strlen(value) + 1));
                strcpy(values[key_count-1], value);
            }else{
                if(parent == nullptr){
                    // --- 这种情况只能是根节点分裂, 它没有父节点 ---
                    //准备新节点
                    node* new_right = new node();
                    node* new_parent = new node();

                    //新的叶子节点修改信息
                    new_right->node_type = leaf;
                    new_right->parent = new_parent;
                    new_right->key_count = this->m - this->m/2;
                    for(int i=0; i<=new_right->key_count-1; i++){
                        new_right->values[i] = this->values[i + this->m/2];
                    }
                    new_right->bt->root_node = new_parent;

                    //新的父亲节点修改信息
                    new_parent->children[0] = this;
                    new_parent->children[1] = new_right;
                    new_parent->node_type = internal_node;
                    new_parent->key_count = 1;
                    new_parent->keys[0] = this->keys[m/2 + 1];
                    new_parent->bt->root_node = new_parent;

                    //当前节点更改自身信息
                    this->next_leaf_node = new_right;
                    this->parent = new_parent;
                    this->node_type = leaf;
                    this->key_count = this->m/2;
                    this->bt->root_node = new_parent;  
                    for(int i=this->m/2; i<=this->m-1; i++){
                        values[i] = nullptr;
                    }                  
                }else{
                    // --- 这种情况叶子节点分裂时, 它是有父节点的 ---
                    
                }
            }
        }

    friend class btree;
};

btree::btree(){
    
}

btree::btree(int m){
    if(pthread_rwlock_init(&this->rwlock, NULL) < 0){
        printf("btree rwlock init error, %s\n", strerror(errno));
        exit(-1);
    }
    this->m = m;
    this->height = 1;

    this->root_node = new node();
    this->root_node->key_count = 0;
    this->root_node->node_type = leaf;
    this->root_node->m = this->m;
    this->root_node->parent = nullptr;
    this->root_node->next_leaf_node = nullptr;
    this->root_node->bt = this;
}

char* btree::Get(uint64_t key){
    char* ans;
    pthread_rwlock_rdlock(&this->rwlock);
    ans = this->root_node->get(key);
    pthread_rwlock_unlock(&this->rwlock);
    return ans;
}

void btree::Insert(uint64_t key, const char * value){
    pthread_rwlock_wrlock(&this->rwlock);
    this->root_node->insert(key, value);
    pthread_rwlock_unlock(&this->rwlock);
}