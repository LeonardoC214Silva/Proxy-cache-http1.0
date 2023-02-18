//
// Created by leo on 19/07/2022.
//

#include "ItemCash.h"

#include <utility>

ItemCash::ItemCash(string ind, string buffer, pthread_t expires) {
    this->index = ind;
    this->buffer = buffer;
    expire = expires;

}

std::string ItemCash::getIndex() {
    return index;
}

std::string ItemCash::getItem() {

    return buffer;
}

void ItemCash::update(string buff, pthread_t expires) {
    this->buffer=buff;
    if(expires != 0);
    pthread_detach(expire);
    expire=expires;
}

void ItemCash::finalizar_depend() {
    if(expire !=0)pthread_detach(expire);
}
