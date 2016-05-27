#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <fcntl.h>

struct message
{
    int size;
    char msg[256];
};

char* readLines(int fd)
{
    char* line="";
    char buf[2];
    int bytesRead;
    strcpy(buf,"");
    buf[0]='\0';

    while ((bytesRead = read(fd,buf,1)) != 0 && buf[0] != '\n')
    {

        if (bytesRead < 0 )
        {
            perror("ERROR (reading from inside the file)");
            exit(3);
        }
        buf[1]='\0';
        strcat(line,buf);
    }

    return line;

}

int main(int argc,char *argv[])
{
    int fd2;
    int fd[2],fd1[2];
    int childPid,childPid1;
    char *readLine="";


    if(argc!=3)
    {
        perror("Invalid number of arguments");
        exit(1);
    }

    struct stat s;
    struct stat s1;
    struct message mes;
    stat(argv[1],&s);
    stat(argv[2],&s1);
    if(!S_ISREG(s.st_mode))
    {
        perror("Not a file");
        exit(1);
    }
    if(!S_ISDIR(s1.st_mode))
    {
        perror("Not a dir");
        exit(2);
    }

    pipe(fd);
    printf("%s",argv[1]);
    char *aux="";
    strcpy(aux,argv[1]);
    if((fd2=open(aux,O_RDWR))<0)
    {
        perror("Error opening file");
        exit(5);
    }
    strcpy(readLine,readLines(fd2));
    mes.size = strlen(readLine);
    strcpy(mes.msg,readLine);

    if(write(fd[1],&mes,sizeof(mes))<0){
        perror("Error wrting in pipe");
        exit(1);
    }

    pipe(fd1);

    childPid = fork();

    if(childPid<0)
    {
        perror("Error creating proccess");
        exit(3);
    }
    else if(childPid>0)
    {
        waitpid(childPid,NULL,0);
    }
    else
    {
        struct message msg1;
        if(read(fd[0],&msg1,sizeof(msg1))<0){
            perror("Error reading from pipe");
            exit(3);
        }
        int *nr=0;
        sscanf(msg1.msg,"%d",nr);
        childPid1 = fork();
        if(childPid1<0)
        {
            perror("Error creating second proccess");
        }
        else if(childPid >0)
        {
            waitpid(childPid1,NULL,0);
        }
        else
        {
        }
    }


    return 0;
}
