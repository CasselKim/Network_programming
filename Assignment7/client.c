#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *message);

int main(int argc, char* argv[]){
        int sock;
        struct sockaddr_in serv_addr;
        char message[BUF_SIZE];

        FILE* readfp;
        FILE* writefp;


        if(argc != 3){
                printf("Usage: %s <IP> <port>\n", argv[0]);
        }
        sock = socket(PF_INET, SOCK_STREAM, 0);
        if(sock==-1)
                error_handling("socket() error");

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
        serv_addr.sin_port = htons(atoi(argv[2]));

        if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
                error_handling("connect() error!");
        else
                puts("Connected........");

        readfp = fdopen(sock, "r");
        writefp = fdopen(sock, "w");

        while(1){
            fputs("Select mode(user/admin) : ", stdout);
            fgets(message, BUF_SIZE, stdin);
            if(!strcmp(message, "user\n") || !strcmp(message, "admin\n"))
                break;        
        }

        fputs(message, writefp);
        fflush(writefp);    
        fgets(message, BUF_SIZE, readfp);
        int admin = strcmp(message, "accept\n");
        if(admin==0)
                printf("[Admin mode] Please input message for clients\n");
        else   
                printf("[User mode] Please wait for admin's message\n");

        while(1){
            if (admin==0){
                fputs("Input message(Q to quit): ", stdout);
                fgets(message, BUF_SIZE, stdin);

                if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
                    break;

                fputs(message, writefp);
                fflush(writefp);
                fgets(message, BUF_SIZE, readfp);
                printf("Message form server : %s", message);
            }
            else{
                while(fgets(message, BUF_SIZE, readfp)==NULL){
                        sleep(1);
                }
                if(!strcmp(message, "quit\n")) break;
                printf("Message form server : %s", message);
            }
        }
        fclose(readfp);
        fclose(writefp);
	return 0;
}



void error_handling(char *message){
        fputs(message, stderr);
        fputc('\n',stderr);
        exit(1);
 }