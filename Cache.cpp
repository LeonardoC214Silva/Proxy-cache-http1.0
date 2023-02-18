//
// Created by leo on 23/07/22.
//

#include "Cache.h"
#include "iostream"
Cache::Cache(int tam, void (*ptr)(const string &, const string &), const string &nomeArq, int alg)
        : tamMax(tam), tamAtual(0) {

    this->nomeArq=nomeArq;
    escreveLog=ptr;
    //algcache=alg;
}

string Cache::buscaItem(const string &url, const string &host) {
    for (auto item:itens) {
        if(item.getHost()==host){
            string busca=item.buscaItem(url);
            conjunto c(url,host);
            {
                unique_lock<mutex> lck(mutLRU);
                atualizaLRU(c);
            }
            (*escreveLog)(nomeArq,"Resposta de Leitura do Cache (Cache Hit)\n");
            return busca;//encontrou o item
        }
    }
    return "";

}

bool Cache::atualizaItem(const string &url, const string &host, const string &novoValor, pthread_t expires) {
    if(novoValor.length()>tamMax){
        (*escreveLog)(nomeArq,"pagina "+host+url+ " maior que o tamanho do cache\n");
        return false;
    }

    for (auto &item:itens) {
        if(item.getHost()==host){
            auto aux=item.getSize()*-1;
            aux+=novoValor.length();
            while (tamAtual + aux > tamMax) {
                //esta cheio cache
                apagaItem(LRU.begin()->url,LRU.begin()->host);
                (*escreveLog)(nomeArq,"pagina "+LRU.begin()->url+LRU.begin()->host+ " por cache cheio\n");
                LRU.erase(LRU.begin());
            }

            if(item.atualizaItem(url, novoValor, expires)) {
               tamAtual+=aux;
               conjunto c(url,host);

               atualizaLRU(c);
               return true;
           }
        }
    }
    return false;
}

bool Cache::inserirItem(const string &url, const string &host, const string &Valor, CacheControl cc, pthread_t expires) {
    if (Valor.length() > tamMax){
        (*escreveLog)(nomeArq,"pagina "+host+url+ " maior que o tamanho do cache\n");
        return false;
    }
    bool retorno = false;
    while (tamAtual + Valor.size() > tamMax) {

        //esta cheio cache
        apagaItem(LRU.begin()->url, LRU.begin()->host);
        (*escreveLog)(nomeArq,"pagina "+LRU.begin()->url+LRU.begin()->host+ " por cache cheio\n");
        LRU.erase(LRU.begin());
    }
    for (auto &item: itens) {
        string resp;
        if (item.getHost() == host) {
            if (item.inserirItem(url, Valor, expires)) {
                retorno=true;
                tamAtual += Valor.size();

               // atualizaLRU(c);
                LRU.emplace_back(url,host);
                if (cc.getAge() > 0) {
                    resp = ("Adicionado " + url + " para Cache," + " deve ser despejado após ") +
                           to_string(cc.getAge()) +
                           "segundos\n";
                    (*escreveLog)(nomeArq, "Adicionado " + url + " para Cache," + " deve ser despejado após " +
                    to_string(cc.getAge()) +"segundos\n");
                } else {
                    if (cc.getExpYear() == 0) {
                        (*escreveLog)(nomeArq, "Adicionado" + url + " para armazenar em " + "cache com cabeçalho não expira\n");
                    } else {

                        (*escreveLog)(nomeArq, "Adicionado " + url + " para armazenar em " + "cache com cabeçalho expira\n");
                    }

                }
            }
            return retorno;
        }


    }
    itens.emplace_back(host);

    if (cc.getAge() > 0) {
        (*escreveLog)(nomeArq, "Adicionado " + url + " para Cache," + " deve ser despejado após " +
                               to_string(cc.getAge()) +"segundos\n");
        retorno = itens.back().inserirItem(url, Valor, expires);
    } else {
        if (cc.getExpYear() == 0) {
            retorno = itens.back().inserirItem(url, Valor, 0);
            (*escreveLog)(nomeArq, "Adicionado" + url + " para armazenar em " + "cache com cabeçalho não expira\n");
        } else {
            retorno = itens.back().inserirItem(url, Valor, expires);
            (*escreveLog)(nomeArq, "Adicionado " + url + " para armazenar em " + "cache com cabeçalho expira\n");
        }

    }




    tamAtual += Valor.size();
    LRU.emplace_back(url,host);
    retorno = true;
    return retorno;

}

void Cache::apagaItem(const string &url, const string &host) {
    if(!itens.empty()) {
        for (auto i = itens.begin(); i != itens.end(); ++i) {
            if (i->getHost() == host) {//encontrou o item que deseja apagar
                int aux = i->getSize();
                if (i->apagaItem(url)) {
                    tamAtual -= aux - i->getSize();

                    for (auto k = LRU.begin(); k <= LRU.end(); k++) {
                        if ((k->host == host) && (k->url == url)) {
                            LRU.erase(k);
                        }
                    }
                }
                if (i->isEmpty())itens.erase(i);//remove do cach o host se necessario
                return;
            }
        }
    }
}

void Cache::atualizaLRU(conjunto indice) {
    for (auto i = LRU.begin();  i<=LRU.end() ; i++) {
        if((i->host==indice.host)&&(i->url==indice.url)){
            LRU.erase(i);
            LRU.emplace_back(indice);
        }
    }
}

string Cache::getConteudo() {
    string retorno;
    for (auto str:itens) {
        retorno+=str.getConteudo();
    }
    return retorno;
}

unsigned long long int Cache::getTamAtual() const {
    return tamAtual;
}
