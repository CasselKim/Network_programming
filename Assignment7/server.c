#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[]){
    int serv_sock;
    int clnt_sock;
    FILE* readFP[100];
    FILE* writeFP[100];
    int admin[100];
    int activate[100];
    int act_size=0;
    int t;
    
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_adr_sz;
    char buf[BUF_SIZE] = {0,};

    struct timeval timeout;
    fd_set reads, cpy_reads;
    int fd_max, fd_num, i;

    if(argc!=2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock==-1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind() error");
    if(listen(serv_sock,5)==-1)
        error_handling("listen() error");

    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;

    while(1){
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        if((fd_num=select(fd_max+1, &cpy_reads, 0,0, &timeout))==-1)
            break;
        if(fd_num==0)
            continue;
        for(i=0;i<fd_max+1;i++){
            if(FD_ISSET(i, &cpy_reads)){
                if(i==serv_sock){
                    clnt_adr_sz = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_adr_sz);
                    if(clnt_sock==-1)
                        error_handling("accept() error!");

                    FD_SET(clnt_sock, &reads);
                    if(fd_max<clnt_sock)
                        fd_max = clnt_sock;

                    activate[act_size++]=clnt_sock;

                    writeFP[clnt_sock] = fdopen(clnt_sock,"w");
                    readFP[clnt_sock] = fdopen(dup(clnt_sock),"r");
                    
                    fgets(buf, BUF_SIZE, readFP[clnt_sock]);
                    if (!strcmp(buf,"admin\n")) {
                        printf("Admin %d has entered..\n",clnt_sock);
                        admin[clnt_sock]=1;
                        fputs("accept\n", writeFP[clnt_sock]);
                        fflush(writeFP[clnt_sock]);
                    }
                    else if (!strcmp(buf,"user\n")) {
                        printf("User %d has entered..\n",clnt_sock);
                        admin[clnt_sock]=0;

                        fputs("deny\n", writeFP[clnt_sock]);
                        fflush(writeFP[clnt_sock]);

                        shutdown(fileno(readFP[clnt_sock]), SHUT_WR);
                        fclose(readFP[clnt_sock]);
                    }
                    else{
                        printf("Wrong input!!\n");
                        fputs("wrong input", writeFP[clnt_sock]);
                        fflush(writeFP[clnt_sock]);
                    }
                }
                else{
                    if(fgets(buf, BUF_SIZE, readFP[i])==NULL){
                        // shut down every socket
                        if(admin[i]){
                            printf("act_size : %d\n",act_size);
                            for(int k=0;k<act_size;k++){
                                t = activate[k];
                                FD_CLR(t, &reads);
                                fputs("quit\n", writeFP[t]);
                                fflush(writeFP[t]);
                                //if(admin[t]){
                                    shutdown(fileno(readFP[t]), SHUT_WR);
                                    fclose(readFP[t]);
                                //}
                                fclose(writeFP[t]);
                                close(t);   
                            }
                            act_size=0;
                            break;
                        }
                    }
                    // send admin's message to all users
                    else if (admin[i]){
                        for(int k=0;k<act_size;k++){
                            t = activate[k];
                            fputs(buf, writeFP[t]);
                            fflush(writeFP[t]);
                        }
                    }
                    else {
                        printf("Unauthorized Access!!");
                        break;
                    }
                }
            }
        }
    }

    close(serv_sock);
    return 0;
}

void error_handling(char *message){
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}