#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char* argv[]){
    int sock;
    int snd_buf = 1024*3;
    int rcv_buf = 1024*3;
    int state;
    socklent_t len;

    sock = socket(PF_INET, SOCK_STREAM, 0);

    state = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_BUF, sizeof(rcv_buf));

    if (state)
        error_handling("setsockopt() error");

    state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_BUF, sizeof(snd_buf));

    if (state)
        error_handling("setsockopt() error");

    len = sizeof(snd_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_BUF, &len);

    if (state)
        error_handling("getsockopt() error");

    len = sizeof(rcv_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_BUF, &len);

    if (state)
        error_handling("getsockopt() error");

    printf("Input buffer size: %d\n",rcv_buf);
    printf("Output buffer size: %d\n",snd_buf);
    return 0;
}


void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}