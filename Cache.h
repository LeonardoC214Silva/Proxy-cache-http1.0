//
// Created by leo on 23/07/22.
//
#include "vector"
#include "ItemCash.h"
#include "CacheHost.h"
#include "CacheControl.h"
#include "mutex"
#ifndef PROXY_CACHE_H
#define PROXY_CACHE_H
struct conjunto{
    string url,host;
    conjunto(string url,string host):url(url),host(host){}
};
typedef struct conjunto conjunto;
using namespace std;
class Cache {
    void (*escreveLog)(const string& arq,const string& texto);
    string nomeArq;
    mutex mutLRU;
    std::vector<CacheHost> itens;
    std::vector<conjunto> LRU;
    unsigned long long tamAtual,tamMax;
    void atualizaLRU(conjunto indice);
public:
    Cache(int tam, void (*ptr)(const string &, const string &), const string &nomeArq, int alg);
    string buscaItem(const string &url, const string &host);
    bool atualizaItem(const string &url, const string &host, const string &novoValor, pthread_t expires);
    bool inserirItem(const string &url, const string &host, const string &Valor, CacheControl cc, pthread_t expires);
    void apagaItem(const string &url, const string &host);

    unsigned long long int getTamAtual() const;//S
    string getConteudo();



};


#endif //PROXY_CACHE_H
