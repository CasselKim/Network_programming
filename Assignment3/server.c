#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>


void error_handling(char *message);

int main(int argc, char *argv[])
{
    int i;
    struct hostent *host;

    int serv_sock, clnt_sock;
    char message[30];
    int option, str_len;
    socklen_t optlen, clnt_adr_sz;
    struct sockaddr_in serv_adr, clnt_adr;
    char* ipaddr;
    char* temp;

    if (argc!=2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock==-1)
        error_handling("socket() Error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)))
        error_handling("bind() error");

    if (listen(serv_sock,5)==-1)
        error_handling("listen() error");

    clnt_adr_sz = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
    
    while((str_len=read(clnt_sock, message, sizeof(message)))!=0)
    {
        printf("from client : %s\n",message);
        for(int ii=0;ii<sizeof(message);ii++){
            if (message[ii]=='\n')
                message[ii]='\0';
        }
        host = gethostbyname(message);
        if (!host)
            error_handling("gethost...error");

        ipaddr = inet_ntoa(*(struct in_addr *)host->h_addr_list[0]);

        write(clnt_sock, ipaddr,str_len);
        write(1,ipaddr, str_len);
    }

    close(clnt_sock);
    close(serv_sock);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}