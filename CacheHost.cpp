//
// Created by leo on 27/07/22.
//

#include "CacheHost.h"

string CacheHost::buscaItem(const string &url) {
    for (auto item:itens) {
        if(item.getIndex()==url){
            return item.getItem();//encontrou o item
        }
    }
    return "";
}

bool CacheHost::atualizaItem(const string &url, const string &novoValor, pthread_t expires) {
    for (auto &item:itens) {
        if(item.getIndex()==url){
            unsigned long tamaux=item.getItem().length();
            tam+=(-1*item.getItem().length())+novoValor.length();
            item.update(novoValor, expires);
            return true;
        }
    }
    return false;
}

bool CacheHost::inserirItem(const string &url, const string &valor, pthread_t expires) {
    bool achou = false;
    for (auto i = itens.begin(); i != itens.end(); ++i) {
        if (i->getIndex() == url) {//encontrou o item que deseja apagar
            achou = true;
        }
    }
    if (!achou) {
        itens.emplace_back(url, valor);//adiciona ao cache caso nao esteja la
        tam += valor.length();
        return true;
    }
    return false;
}

bool CacheHost::apagaItem(const string &url) {
    for (auto i = itens.begin(); i != itens.end(); ++i) {
        if(i->getIndex()==url){//encontrou o item que deseja apagar
            tam-=i->getItem().length();
            i->finalizar_depend();
            itens.erase(i);//remove do cache
            return true;
        }
    }
    return false;
}

bool CacheHost::isEmpty() {
    return itens.empty();
}

string CacheHost::getConteudo() {
    string retorno;
    for (auto str:itens) {
        retorno+="{"+str.getItem()+"}\n";
    }
    return retorno;
}

CacheHost::CacheHost(const string &host) : host(host) {
    tam=0;
}

void CacheHost::setHost(const string &host) {
    CacheHost::host = host;
}

unsigned long long int CacheHost::getSize() {
    return tam;
}

string CacheHost::getHost() {
    return host;
}
