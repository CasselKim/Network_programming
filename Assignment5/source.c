#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100

void error_handling(char *message);
int main(int argc, char *argv[]){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout;
    fd_set reads, cpy_reads;

    int fd[10];
    int fd_size=0;

    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];

    char welcome[10] = "Welcome~\n";
    char client_num[60];
    char CLIENT_HELLO = '0';
    char CLIENT_CHAT = '1';
    char CLIENT_BYE = '2';
    char client_id[3];
    char temp[BUF_SIZE];


    if(argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));
    if (bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
        error_handling("bind() error");
    if (listen(serv_sock, 5)==-1)
        error_handling("listen() error");

    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;
    while(1){
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        if((fd_num = select(fd_max+1, &cpy_reads, 0,0, &timeout))==-1)
            break;
        if(fd_num==0)
            continue;
        for(i=0;i<fd_max+1;i++){
            if(FD_ISSET(i, &cpy_reads)){
                if(i==serv_sock){
                    adr_sz = sizeof(clnt_adr);
                    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
                    FD_SET(clnt_sock, &reads);
                    if(fd_max<clnt_sock)
                        fd_max = clnt_sock;
                    fd[fd_size++]=clnt_sock;
                    printf("connected client: %d\n",clnt_sock);
                
                    // send welcome message
                    write(clnt_sock, welcome, strlen(welcome));
                    sleep(1);

                    // send the number  of clients
                    sprintf(buf, "The number of Clients is %d now.\n",fd_size);
                    write(clnt_sock, buf,strlen(buf));

                    sprintf(client_id, "%02d",clnt_sock);
                    buf[0] = CLIENT_HELLO;
                    strcpy(buf+1,client_id);
                    strcpy(buf+3,welcome);

                    for(int j=0 ;j<fd_size-1;j++){
                        write(fd[j], buf, strlen(buf));
                    } 
                }
                else{
                    str_len = read(i, buf, BUF_SIZE);
                    buf[str_len]='\0';
                    if(str_len==0){

                        // remove i from fd[]
                        for(int k=0;k<fd_size;k++){
                            if(fd[k]==i){
                                for(int m=k;m<fd_size-1;m++)
                                    fd[m]=fd[m+1];
                                }
                            break;
                        }
                        
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client: %d\n",i);

                        fd_size--;
                        fd[fd_size]=-1;

                        sprintf(client_id, "%02d",i);
                        buf[0] = CLIENT_BYE;
                        strcpy(buf+1,client_id);
                        strcpy(buf+3,welcome);
                        
                        for(int j=0 ;j<fd_size;j++){
                            write(fd[j], buf, strlen(buf));
                        }
                    }
                    else{
                        strcpy(temp,buf);
                        sprintf(client_id, "%02d",i);
                        buf[0] = CLIENT_CHAT;
                        strcpy(buf+1,client_id);
                        strcpy(buf+3,temp);

                        for(int j=0 ;j<fd_size;j++){
                            if (fd[j]!=i){
                                write(fd[j], buf, strlen(buf));
                            }
                        } 
                    }
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}
void error_handling(char *buf){
    fputs(buf, stderr);
    fputc('\n',stderr);
    exit(1);
}