#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
#define OPSZ 4
#define RLT_SIZE 4

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sock;
    char opmsg[BUF_SIZE];
    int result, opnd_cnt, i;
    int str_len;
    socklen_t addr_sz;
    struct sockaddr_in serv_addr, from_addr;
    
    if (argc != 3)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
                error_handling("connect() error!");
    else
            puts("Connected........");

    while (1)
    {
        fputs("Operant count (-1 to quit) : ", stdout);
        scanf("%d", &opnd_cnt);

        if (opnd_cnt==-1)
            break;

        opmsg[0] = (char)opnd_cnt;
        for (i = 0; i < opnd_cnt; i++)
        {
            printf("Operant %d: ", i + 1);
            scanf("%d", (int *)&opmsg[i * OPSZ + 1]);
        }
        fgetc(stdin);
        fputs("Operator : ", stdout);
        scanf("%c", &opmsg[opnd_cnt * OPSZ + 1]);

        write(sock, opmsg, opnd_cnt * OPSZ + 2);
        str_len = read(sock, &result, RLT_SIZE);
        printf("Operation result : %d\n", result);
    }
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}