
//cliente usa fork para gerar os processos filhos
//executar usando um parametro que indique a quantidade de filhos
//exemplo ./cliente 5
#include <stdlib.h>
#include <stdio.h>
#include<unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include<sys/wait.h>
#include<arpa/inet.h>
#include "string"

using namespace std;
void funcao_filho(int numfilho,int porta,string myip,int info);
int main(int argc,char** argv) {
    int port= atoi(argv[1]);
    int info= atoi(argv[3]);
    string ip=argv[2];

    srand(time(nullptr));

    funcao_filho(1,port,ip,info);

    wait(NULL);//esperando a execução dos filhos


    return 0;
}
void funcao_filho(int numfilho,int porta = 1972,string myip="127.0.0.1",int info=0){
    string mensagens="INFO "+ to_string(info);
    int sock;

    struct sockaddr_in sAddr;
    char buffer[25];
    char b2[5000];
    int x;
    sAddr.sin_family = AF_INET;
    sAddr.sin_addr.s_addr = INADDR_ANY;
    sAddr.sin_port = 0;
    sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    bind(sock,(const struct sockaddr *)&sAddr, sizeof(sAddr));
    sleep(2);
    sAddr.sin_addr.s_addr = inet_addr(myip.c_str());//endereço para rodar na maq local
    sAddr.sin_port=htons(porta);
    if(connect(sock,(const struct sockaddr *)&sAddr, sizeof(sAddr))!=0){
        perror("cliente!");
        return;
    }

    snprintf(b2,4999,mensagens.c_str());

    sleep(1);// essa pausa ajuda na espera da conexao e desconexão dos clientes
    printf("filho#%i, mandou %i caracteres\n",numfilho,send(sock,b2, strlen(b2),0));

    close(sock);
}

