//
// Created by leo on 07/08/22.
//

#include "Estatic.h"

Estatic::Estatic(Cache *cache, string nomeLog) {
    int resultado;
    this->cache=cache;
    this->nomeArq=nomeLog;
    tam_Med_Pag=0;
    nPedidosTotal=nped=hitTotal=hits=faltaTotal=faltas=0;
    P_Ult_Sec=hit_Last_Sec=n_cli_Ult_sec=n_serv_Ult_sec=0;
    id_media_pag=0;
    porcPagExp=porcPAgDesp_cab=0;
    dur_med_exp=0;

    if(resultado!=0){
        printf("erro ao criar a thread!\n");

    }


}

void Estatic::incPedido() {
    nPedidosTotal++;
    std::unique_lock<std::mutex> lck1(mutPe);
    nped++;
}

void Estatic::incFalta() {
    std::unique_lock<std::mutex> lck1 (mutFalta);
    faltaTotal++;
    faltas++;
}

void Estatic::incHit() {
    std::unique_lock<std::mutex> lck1 (mutHit);
    hitTotal++;
    hits++;

}

void Estatic::pedSec() {
    while(true) {
        sleep(1);
        {
            std::unique_lock<std::mutex> lck1(mutPe);
            P_Ult_Sec = nped;
            nped = 0;
        }
    }
}

void Estatic::hitSec() {
    while(1) {
        sleep(1);
        {
            std::unique_lock<std::mutex> lck1(mutHit);
            hit_Last_Sec = hits;
            hits = 0;
        }
    }
}

void Estatic::faltSec() {
    while(1) {
        sleep(1);
        {
            std::unique_lock<std::mutex> lck1(mutFalta);
            falta_Last_Sec = faltas;
            faltas = 0;
        }
    }

}

void Estatic::cliSec() {
    while (true) {
        sleep(1);
        {
            std::unique_lock<std::mutex> lck1(mutCli);
            n_cli_Ult_sec = n_cli;
            n_cli = 0;
        }
    }

}

void Estatic::servSec() {

    while(true) {
        sleep(1);
        {
            std::unique_lock<std::mutex> lck1(mutServ);
            n_serv_Ult_sec = n_serv;
            n_serv = 0;
        }
    }
}

void Estatic::incExp() {
    std::unique_lock<std::mutex> lck1 (mutExp);
    cabExp++;

}


float Estatic::getPercExp() {
    porcPAgDesp_cab=(float)(cabExp/nPedidosTotal);
    porcPagExp=porcPAgDesp_cab;
    return porcPAgDesp_cab;
}

string Estatic::info(int valor) {
    string mens;
    if (valor == 0) {
    mens="tamanho usado do cache: "+to_string(cache->getTamAtual())+"\n";
    mens+="itens em cache{"+cache->getConteudo()+"}\n";
    return mens;
    }
    if(valor==1){
        mens="\nnporcentagem paginas expiradas:"+to_string(porcPAgDesp_cab);
        mens="\nnumero de hits:"+to_string(hitTotal);
        mens+="\nnumero de hits ultimo segundo:"+ to_string(hit_Last_Sec);
        mens+="\nnumero de pedidos:"+to_string(nPedidosTotal);
        mens+="\nnumero de pedidos ultimo segundo:"+to_string(P_Ult_Sec);
        mens+="\nnumero de faltas:"+ to_string(faltaTotal);
        mens+="\nnumero de faltas ultimo segundo:"+ to_string(falta_Last_Sec);
        mens+="\nnumero de clientes ultimo segundo:"+ to_string(n_cli_Ult_sec);
        mens+="\nnumero de solicitações ao servidor no ultimo segundo:"+ to_string(n_serv_Ult_sec)+"\n";
        return mens;

    }
    return "";

}

Estatic::~Estatic() {

}
