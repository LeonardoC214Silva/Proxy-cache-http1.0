//
// Created by leo on 19/07/2022.
//

#ifndef PROXY_ITEMCASH_H
#define PROXY_ITEMCASH_H
#include "string"
#include "stdexcept"
#include "pthread.h"
using namespace std;
class ItemCash {
 std::string index;
 std::string buffer;
 pthread_t expire;
public:
    ItemCash(string ind, string buffer, pthread_t expires=0);
    std::string getIndex();
    void update(string buff, pthread_t expires);
    std::string getItem();
    void finalizar_depend();
};


#endif //PROXY_CACHEHOST_H
