//
// Created by leo on 27/07/22.
//

#ifndef PROXY_CACHEHOST_H
#define PROXY_CACHEHOST_H
#include "ItemCash.h"
#include "vector"
#include "string"

using namespace std;
class CacheHost {
    string host;
    vector<ItemCash> itens;
   unsigned long long tam;


public:
    CacheHost(const string &host);

    void setHost(const string &host);

    string buscaItem(const string& url);
    bool atualizaItem(const string &url, const string &novoValor, pthread_t expires);
    bool inserirItem(const string &url, const string &valor, pthread_t expires);
    bool apagaItem(const string& url);
    bool isEmpty();
    string getConteudo();
    string getHost();
    unsigned long long int getSize();

};


#endif //PROXY_CACHEHOST_H
