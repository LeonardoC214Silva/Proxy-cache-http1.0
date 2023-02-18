//
// Created by leo on 07/08/22.
//

#ifndef PROXY_ESTATIC_H
#define PROXY_ESTATIC_H
#include <thread>
#include "Cache.h"
#include "string"
#include "mutex"
#include <unistd.h>
using namespace std;
class Estatic {
    Cache* cache;
    string nomeArq;
    thread atthit;
    unsigned long long int nPedidosTotal,nped,hitTotal,hits,faltaTotal,faltas,n_cli,n_serv;
    unsigned long long int cabExp;
    unsigned  long long P_Ult_Sec,hit_Last_Sec,falta_Last_Sec,n_cli_Ult_sec,n_serv_Ult_sec;
    mutex mutPe,mutHit,mutFalta,mutCli,mutServ,mutExp;
    unsigned long long id_media_pag,tam_Med_Pag;
    float porcPagExp,porcPAgDesp_cab;
    unsigned long long dur_med_exp;
    pthread_t thread_id;

public:
    Estatic(Cache *cache, string nomeLog);

    virtual ~Estatic();
    void incExp();
    void incPedido();
    void incFalta();
    void incHit();
    void pedSec();
    void hitSec();
    void faltSec();
    void cliSec();
    void servSec();
    float getPercExp();
    string info(int valor);

};


#endif //PROXY_ESTATIC_H
