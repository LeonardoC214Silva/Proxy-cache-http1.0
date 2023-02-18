
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
void funcao_filho(int numfilho,int porta,string myip);
int main(int argc,char** argv) {
    int port= atoi(argv[2]);
    string ip=argv[3];
    srand(time(nullptr));
    int nfilhos=1;
    int pid;//identificador do processo
    int x;//usaremos no for
    if(argc>1){
        nfilhos = atoi(argv[1]);
    }
    for (x = 0; x < nfilhos; x++) {
        if((pid = fork())==0){
            funcao_filho(x+1,port,ip);
            exit(0);
        }
        sleep(1);
    }//fim do for :)
    wait(NULL);//esperando a execução dos filhos


    return 0;
}
void funcao_filho(int numfilho,int porta = 1972,string myip="127.0.0.1"){
    string mensagens[]={"GET /wireshark-labs/HTTP-wireshark-file3.html HTTP/1.1\r\nHost: gaia.cs.umass.edu\r\nmax-age= 5\r\nConnection: keep-alive\r\n\r\n",
                        "GET /wireshark-labs/HTTP-wireshark-file5.html HTTP/1.1\r\nHost: gaia.cs.umass.edu\r\nConnection: keep-alive\r\n\r\n",
                        "GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: keep-alive\r\n\r\n"};
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
    sAddr.sin_addr.s_addr = inet_addr(myip.c_str());//endereço para rodar na maq localS
    sAddr.sin_port=htons(porta);
    if(connect(sock,(const struct sockaddr *)&sAddr, sizeof(sAddr))!=0){
        perror("cliente!");
        return;
    }
    string teste=mensagens[rand()%2];
    snprintf(b2,4999,teste.c_str());
  //  snprintf(b2,4999,"INFO 0");


    //                   "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n\r\n");
    sleep(1);// essa pausa ajuda na espera da conexao e desconexão dos clientes
    printf("filho#%i, mandou %i caracteres\n",numfilho,send(sock,b2, strlen(b2),0));


    string resp;
    while ((x = (int) recv(sock, b2, 4999, 0)) > 0) {//recebe solicitação
        b2[x] = '\0';//garante que nao ira considerar o lixo do buffer
        resp += b2;
        //    cout<<mens<<"\n";
    }

    printf("\nresposta:%s\n",resp.c_str());
    close(sock);
}

