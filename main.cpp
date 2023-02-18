#include <iostream>
#include <pthread.h>
#include "thread"
#include "string.h"
#include "Cache.h"
#include <mutex>
#include "ctime"
#include "Estatic.h"
#if WIN32//libs para windows
#include<winsock2.h>
#include<windows.h>
#pragma comment(lib, "ws2_32")
#include <ws2tcpip.h>
#include <winsock.h>

WSADATA wsa;
#else//libs para linux
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include<unistd.h>

#include<arpa/inet.h>
#include "netdb.h"
#endif
using namespace std;
//variaveis globais
mutex escArqMut;
mutex mutEsc,mutLei,mutContLei;
unique_lock<std::mutex> lckEsc;
int contLei=0;
Cache *cache;
Estatic *myEst;
struct param {
    int socket,soclis;//socket criado
    string nomelog;//arquivo de log

};
///funcoes usadas
string buscaHost(const string& request);
string getUrl(const string& request);
void * thread_proc(void *arg);
void escritaLog(const string& arq,const string& texto);
string buscaServ(string mens, string nomeLog);
void printLogFazerSolict(const string& mens,const string& nomeLog);
void printLogRecSolictCli(const string& mens,const string& nomeLog);
void pagAge(int age, string message, string nomeArq);
void pagExpira(CacheControl cc, string mensagem, string nomeArq);

int main(int argc,char** argv) {
#if WIN32//inicialização para windows
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
#endif
    lckEsc=unique_lock<std::mutex>(mutEsc);
    if(lckEsc.owns_lock())lckEsc.unlock();
    int cache_size=1;
    struct param* dados;
    int porta=1972;//porta padrao
    int algcache;
    string nomelog;
    for(int i=1;i<argc;i++){//iniciando valores com parametros
        if(argv[i][0]=='-'){
            if((i<argc-1) && (argv[i+1][0]!='-')){//garantir que exite um argumento a frente
                if(argv[i][1]=='c'){
                    cache_size=(int)strtol(argv[i+1], nullptr,10);
                }
                if(argv[i][1]=='p'){
                    porta=(int)strtol(argv[i+1], nullptr,10);
                }
                if(argv[i][1]=='l'){
                    nomelog=argv[i+1];
                }
                if(argv[i][1]=='a'){
                    algcache=(int)strtol(argv[i+1], nullptr,10);
                }

            }
        }
    }
    Cache servCache(cache_size * 1000, escritaLog, nomelog, algcache);
    cache = &servCache;
    Estatic est(&servCache,nomelog);
    std::thread t1(&Estatic::hitSec, &est);
    std::thread t2(&Estatic::cliSec, &est);
    std::thread t3(&Estatic::faltSec, &est);
    std::thread t4(&Estatic::pedSec, &est);
    std::thread t5(&Estatic::servSec, &est);
    myEst=&est;
    struct sockaddr_in sAddr;
    int listensock,novo_sock,resultado;
    int valor;
    pthread_t thread_id;
    valor = 1;
    listensock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    resultado = setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&valor), sizeof(valor));
    if(resultado <0){
        perror("erro_setsockopt");
        return 1;
    }
    sAddr.sin_family= AF_INET;
    sAddr.sin_port=htons(porta);
    sAddr.sin_addr.s_addr=INADDR_ANY;
    resultado = bind(listensock,(struct sockaddr *)&sAddr, sizeof(sAddr));
    if(resultado<0){
        perror("erro bind");
        return 1;
    }
    resultado = listen(listensock,10);
    if(resultado <0){
        perror("erro listen");
        return 1;
    }
    while(1){
        /**
         * uma vez que o cliente conecta uma nova therad é iniciada o descritor para
         * esse novo socket é passado para a funcao thread apos passado para a funcao
         * nao ha necessidade da phtread pai fechar o descritor
         */
        novo_sock = accept(listensock,nullptr,nullptr);//aceita clientes
        //dados que a thread ira usar
        dados = new struct param;
        dados->socket=novo_sock;
        dados->nomelog=nomelog;
        //cria a thread
        resultado = pthread_create(&thread_id,nullptr,thread_proc,dados);
        if(resultado!=0){
            printf("erro ao criar a thread!\n");
            return 1;
        }
      //  pthread_join(thread_id,nullptr);
        //usado para eviar a criação de threads zumbies :)
     //   pthread_detach(thread_id);
        /**usada para dar a  thread a chance de inicializar a execução desisitindo
        * do restante do intervalo de tempo alocado dos pais
        */
       // sched_yield();
    }

    return 0;
}
#define tam  3000
void * thread_proc(void *arg) {


    int sockCli;
   // Cache *cache;
    char buffer[tam];
    memset(buffer, '\0', tam);//zera buffer
    char *cstr, *ptr;
    int n_lidos;
    size_t pos;
    string mens, respCli,url,textolog,nomeLog;

    sockCli = ((struct param *) arg)->socket;

    nomeLog = ((struct param *) arg)->nomelog;



    printf("Thread filho %i com pid %i criada.\n", pthread_self(), getpid());


    if ((n_lidos = (int) recv(sockCli, buffer, tam - 1, 0)) < 0) {//recebe solicitação
        perror("erro ao receber");
    }
    buffer[n_lidos] = '\0';//garante que nao ira considerar o lixo do buffer
    mens += buffer;

    asm volatile("" ::: "memory");//evitar que as instruçoes sejam trocadas em tempo de execuçao
    //send(sockCli,&mens,n_lidos,0);
    ///tratando solicitaçao
    cstr = new char[mens.length() + 1];
    strcpy(cstr, mens.c_str());
    ptr = strtok(cstr, " ");
    if (strcmp(ptr, "GET") == 0) {
        myEst->incPedido();
        //é um get
        strcpy(cstr, mens.c_str());//restaura string

        CacheControl cc(mens);

        //leitura cache
        //pedindo vez no mutex
        {
            unique_lock<std::mutex> lck(mutContLei);
            if(!lck.owns_lock())lck.lock();
            contLei++;
            if (contLei == 1) {
                lckEsc.lock();
            }
        }
        respCli= cache->buscaItem(getUrl(mens), buscaHost(mens));
        //liberando leitor
        {
            unique_lock<std::mutex> lck(mutContLei);
            if(!lck.owns_lock())lck.lock();
            contLei--;
            if (contLei == 0) {
                lckEsc.unlock();//liberando escrita
            }
        }

        if(!respCli.empty()){//existe algo no cache

            if (cc.isIfModSince()) {//caso nao tenha If-Modified-Since

                string respcache = respCli;
                respCli = buscaServ(mens, nomeLog);
                if (respCli == "HTTP/1.0 304 Not Modified") {
                    cout << "nao mudou\n";
                    respCli = respcache;
                } else {
                    unique_lock<std::mutex> lck(mutEsc);//pede a vez do mutex de escrita
                    if(!lck.owns_lock())lck.lock();
                    if (cc.getAge() > 0 || cc.getExpYear() > 0)
                        cache->atualizaItem(getUrl(mens), buscaHost(mens), respCli, pthread_self());
                    else cache->atualizaItem(getUrl(mens), buscaHost(mens), respCli, 0);
                }
            }
            myEst->incHit();
            cout<<" pegou cache\n";
        } else{//ocorre se nao estiver emm cache

            myEst->incFalta();//incrementa faltas
            respCli = buscaServ(mens,nomeLog);
            CacheControl cc1(respCli);
            if(!cc.isNoCache()) {
                unique_lock<std::mutex> lck (mutEsc);
                if(!lck.owns_lock())lck.lock();
                if (cc.getAge() > 0 || cc.getExpYear() > 0||cc1.getAge() > 0 || cc1.getExpYear() > 0)cache->inserirItem(getUrl(mens), buscaHost(mens), respCli, cc, pthread_self());
                else cache->inserirItem(getUrl(mens), buscaHost(mens), respCli, cc, 0);

            }else{
                escritaLog(nomeLog,"nao armazenado "+ buscaHost(mens)+ getUrl(mens)+" devido a cabeçalho no cache\n");
            }
            cout<<"busc serv";
        }

        //envia pagina para o cliente
        if(send(sockCli, respCli.c_str(), respCli.length(), 0)<0)perror("erro enviar cleinte");

    } else {
        string str2=mens;
        str2.erase(5,string::npos);

        if(str2=="INFO "){

            string mens2;
            size_t positon= mens.find(' ');
            mens2=mens.c_str()+positon+1;
            int aux;

            aux=stoi(mens2,nullptr,10);
            cout<<mens<<"\n "<<to_string(aux);
            escritaLog(nomeLog,myEst->info(aux));

        }else {
            //nao e um get, reportando erro
            send(sockCli,
                 "HTTP/1.0 501 Not Implemented!\r\nContent-Type: text/html\r\nContent-Length: 2345\r\n\n <HTML><BODY></BODY> 501 Not Implemented!</HTML>",
                 strlen("HTTP/1.0 501 Not Implemented!\r\nContent-Type: text/html\r\nContent-Length: 2345\r\n\n <HTML><BODY></BODY> 501 Not Implemented!</HTML>"),
                 0);
        }
    }
    delete cstr;


    close(sockCli);
    delete (struct param *) arg;//desaloca parametro
    printf("Thread filha %i com pid %i finalizada.\n", pthread_self(), getpid());
    CacheControl c1(mens);
    CacheControl c2(respCli);
    if(c1.getAge()>0){
        pagAge(c1.getAge(), mens, nomeLog);
    }else{
        if(c2.getAge()>0){
            pagAge(c2.getAge(), mens, nomeLog);
        }else{
            if(c1.getExpYear()>0){
                pagExpira(c1, mens, nomeLog);
            }else{
                if(c2.getExpYear()>0){
                    pagExpira(c2, mens, nomeLog);
                }
            }
        }
    }

}
string buscaHost(const string& request) {
    char *cstr = new char[request.length() + 1];
    strcpy(cstr, request.c_str());
    string host;
    size_t pos;
    char aux[16];//tamanho do endereço ip com o \0
    aux[15]='\0';
    if (!request.empty()) {

        pos = request.find("Host: ");
        if (pos == std::string::npos){
            printf("mensagem com formato incorreto\n");
            exit(1);
        }
        host = cstr + pos + 6;
        pos=host.find('\r');
        host.erase(pos,std::string::npos);

        if (!isdigit(host[0])) {
            struct hostent *maquina;//entidade host
            struct in_addr **addr_list;
            //recupera o ip pelo nome
            if ((maquina = gethostbyname(host.c_str())) == nullptr) {
                printf("erro ao pegar ip do host\n");

                exit(1);
            } else {
                addr_list = (struct in_addr **) maquina->h_addr_list;
                for (int j = 0; addr_list[j] != nullptr; ++j) {
                    strcpy(aux, inet_ntoa(*(addr_list[j])));

                }
            }
        }
        host=aux;
        delete[] cstr;
        return host;
    }
    return "";
}
string getUrl(const string& request){
    char* aux,*ptr;
    string url;
    size_t pos;
    aux= new char[request.length()+1];
    strcpy(aux,request.c_str());
   ptr=aux+4;//pula o 'GET '
    url= ptr;
    pos=url.find(' ');
    url.erase(pos,string::npos);
    delete []aux;
    return url;
}
void escritaLog(const string& arq,const string& texto){
    FILE *fp;
    unique_lock<std::mutex> lckLog(escArqMut);//pede por mutex do log
    while((fp= fopen(arq.c_str(),"a"))== nullptr);
    fwrite(texto.c_str(),1,texto.length(),fp);
    fclose(fp);

}
string buscaServ(string mens, string nomeLog) {
    int sockServ,n_lidos;
    char *cstr, *ptr,buffer[tam];
    string textolog,respCli;
    //verificar se eh http/1.1
    size_t loc= mens.find("HTTP/1.1");
    if(loc!=string::npos){
        mens.replace(loc,8,"HTTP/1.0");
    }

    string host = buscaHost(mens);
    struct sockaddr_in sAddr;
    //atualizando log
    printLogRecSolictCli(mens,nomeLog);

    //preparando conexão com o servidor desejado
    sAddr.sin_family = AF_INET;
    sAddr.sin_addr.s_addr = inet_addr(host.c_str());//endereço para rodar na maq local
    sAddr.sin_port = 0;
    sockServ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bind(sockServ, (const struct sockaddr *) &sAddr, sizeof(sAddr));
    sAddr.sin_family = AF_INET;
    sAddr.sin_addr.s_addr = inet_addr(host.c_str());//endereço para rodar na maq local
    sAddr.sin_port = htons(80);
    //conectando com o servidor
    if (connect(sockServ, (const struct sockaddr *) &sAddr, sizeof(sAddr)) != 0) {
        perror("servidor com pagina!");
    }

    sleep(1);// essa pausa ajuda na espera da conexao e desconexão dos clientes
    ///atualizando log
    printLogFazerSolict(mens,nomeLog);

    //cout<<"soli:\n"<<mens<<"\n\n";
    if(send(sockServ, mens.c_str(), mens.length(), 0)<0) {//solicitando pagina
    perror("erro ao solicitar ao servidor");
    }

    while ((n_lidos = (int) recv(sockServ, buffer, tam - 1, 0)) > 0) {//recebe solicitação
        buffer[n_lidos] = '\0';//garante que nao ira considerar o lixo do buffer
        respCli += buffer;
        //    cout<<mens<<"\n";
    }
    if(n_lidos<0){
        perror("erro ao enviar ao receber do servidor");
    }

    return respCli;
}
void printLogFazerSolict(const string& mens,const string& nomeLog){
    string textolog;
    textolog = "Solicitar ";
    textolog += getUrl(mens);
    textolog += " de ";
    textolog += buscaHost(mens);
    textolog += ".\n";
    escritaLog(nomeLog, textolog);
}
void printLogRecSolictCli(const string& mens,const string& nomeLog){
    string textolog;
    textolog = "Solicitacao de ";
    textolog += buscaHost(mens);
    textolog += getUrl(mens);
    textolog += " de ";
    textolog += to_string(pthread_self());
    textolog += ".\n";
    escritaLog(nomeLog, textolog);
}
void pagAge(int age, string message, string nomeArq) {
    sleep(age);
     unique_lock<std::mutex> lck(mutEsc);
    if(!lck.owns_lock())lck.lock();
    cache->apagaItem(getUrl(message), buscaHost(message));
    myEst->incExp();
    escritaLog(nomeArq,"removido "+ buscaHost(message)+ getUrl(message)+" por cabeçalho max-age\n");
}
void pagExpira(CacheControl cc, string mensagem, string nomeArq) {
    bool expirou;
    time_t rawtime;
    struct tm * t_info;
    expirou = false;
    while(!expirou){
        sleep(60);
        time (&rawtime);
        t_info = localtime(&rawtime);
        if(cc.getExpYear()==t_info->tm_year){
            if(cc.getExpMonth()==t_info->tm_mon){
                if(cc.getExpDay()==t_info->tm_mday){
                    if(cc.getExpHour()==t_info->tm_hour){
                        if((cc.getExpMin()==t_info->tm_min)){
                            expirou=true;
                             unique_lock<std::mutex> lck(mutEsc);
                             if(!lck.owns_lock())lck.lock();
                                 cache->apagaItem(getUrl(mensagem), buscaHost(mensagem));

                             myEst->incExp();
                            escritaLog(nomeArq,"removido "+ buscaHost(mensagem)+ getUrl(mensagem)+" por cabeçalho expires\n");
                        }
                    }
                }
            }
        }
    }


}