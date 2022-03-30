#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define OPSZ 4

void error_handling(char *message);
int calculate(int opnum, int opnds[], char op);

int main(int argc, char *argv[])
{
    int serv_sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t clnt_addr_size;
    char opinfo[BUF_SIZE];
    int result, opnd_cnt, i;
    int recv_cnt, recv_len;
    struct sockaddr_in serv_addr, clnt_addr;
    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (serv_sock == -1)
        error_handling("UDP socker creation error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error!");

    while (1)
    {
        opnd_cnt = 0;
        clnt_addr_size = sizeof(clnt_addr);
    
        str_len = recvfrom(serv_sock, opinfo, BUF_SIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        opnd_cnt = opinfo[0];
        recv_len = 0;
        for(i=1;i<opnd_cnt*OPSZ+1;i=i+4){
            printf("%d\n",opinfo[i]);
        }
        result = calculate(opnd_cnt, (int *)(opinfo+1), opinfo[str_len - 1]);
        sendto(serv_sock, (char *)&result, OPSZ, 0, (struct sockaddr*)&clnt_addr, clnt_addr_size);
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int calculate(int opnum, int opnds[], char op)
{
    int result = opnds[0], i;
    switch (op)
    {
    case '+':
        for (i = 1; i < opnum; i++)
            result += opnds[i];
        break;
    case '-':
        for (i = 1; i < opnum; i++)
            result -= opnds[i];
        break;
    case '*':
        for (i = 1; i < opnum; i++)
            result *= opnds[i];
        break;
    case '^':
        for (i = 1; i < opnum; i++)
            result = pow(result,opnds[i]);
        break;
    }
    return result;
}