#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
#define CLIENT_HELLO 0
#define CLIENT_CHAT 1
#define CLIENT_BYE 2


void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

int main(int argc, char *argv[]){
    int sock;
    pid_t pid;
    char buf[BUF_SIZE];
    char buf2[BUF_SIZE];
    struct sockaddr_in serv_adr;

    if(argc!=3){
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
        error_handling("connect() error!");
    
    // read socket form : [message] # Welcome~
    int str_len;

    str_len = read(sock, buf, BUF_SIZE-1);
    buf[str_len]=0;
    printf("Server : %s", buf);

    // read socket form : [message] # The number of Clients is 00 now.
    str_len = read(sock, buf, BUF_SIZE-1);
    buf[str_len]=0;
    printf("Server : %s", buf);

    pid = fork();
    if(pid==0)
        write_routine(sock, buf);
    else
        read_routine(sock, buf);
    close(sock);
    return 0;
}

void read_routine(int sock, char *buf){
    while(1){
        // read socket form : [state, clientID, message]
        int str_len = read(sock, buf, BUF_SIZE-1);
        if(str_len==0)
            return ;
        buf[str_len]=0;

        int state = buf[0]-'0';
        int clientID = (buf[1]-'0')*10+(buf[2]-'0');
        char *message = buf+3;

        if(state==CLIENT_HELLO){
            printf("Client %d has joined this chatting room\n", clientID);
        }
        else if(state==CLIENT_CHAT){
            printf("Client %d : %s", clientID, message);
        }
        else if(state==CLIENT_BYE){
            printf("Client %d has left this chatting room\n", clientID);
        }
    }
}

void write_routine(int sock, char *buf){
    while(1){
        fgets(buf, BUF_SIZE, stdin);
        if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")){
            shutdown(sock, SHUT_WR);
            return ;
        }
        write(sock, buf, strlen(buf));
    }
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}