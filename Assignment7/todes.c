#include <stdio.h>
#include <fcntl.h>

int main(){
    FILE *fp;
    int fd=open("data.dat", O_WRONLY|O_CREAT|O_TRUNC);
    if(fd==-1){
        fputs("file open error",stdout);
        return -1;
    }

    printf("Filst file descriptor : %d\n",fd);
    fp=fdopen(fd,"w");
    fputs("TCP socket Programming\n",fp);
    printf("Second file descriptor : %d\n",fileno(fp));
    fclose(fp);
    return 0;
}