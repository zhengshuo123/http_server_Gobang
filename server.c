#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BACKLOG 1

int make_server_socket_q(int,int);
int make_server_socket(int portnum);
void read_til_crnl(FILE * fp);
void process_rq(char *rq,int fd);
void cannot_do(int fd);
void do_404(char *item,int fd);
int not_exist(char *f);
void do_cat(char *f,int fd);

int main(int ac,char *av[])
{
    int sock,fd;
    FILE *fpin;
    char request[BUFSIZ];

    if(ac == 1){
        fprintf(stderr,"usage: ws portnum\n");
        exit(1);
    }

    sock = make_server_socket(atoi(av[1]));
    if(sock == -1) exit(2);

    while(1){
        fd = accept(sock,NULL,NULL);
        fpin = fdopen(fd,"r");

        fgets(request,BUFSIZ,fpin);
        printf("got a call: request = %s",request);
        read_til_crnl(fpin);

        process_rq(request,fd);

        fclose(fpin);
        close(fd);
    }
}

void read_til_crnl(FILE * fp)
{
    char buf[BUFSIZ];
    while(fgets(buf,BUFSIZ,fp) != NULL && strcmp(buf,"\r\n") != 0);
}

void process_rq(char *rq,int fd)
{
    int status;
    char cmd[BUFSIZ],arg[BUFSIZ];
    if(fork() != 0){
        while(waitpid(-1, &status, WNOHANG));
        return;
    }
    strcpy(arg,"./");
    if(sscanf(rq,"%s /%s",cmd,arg+2)!=2)
        return;
    printf("%s,%s\n",cmd,arg);
    if(strcmp(cmd,"GET") != 0)
        cannot_do(fd);
    else if(not_exist(arg))
        do_404(arg,fd);
    else
        do_cat(arg,fd);
    exit(0);
}

void header(FILE *fp, char *content_type)
{
    if (fp != NULL) {
        fprintf(fp, "HTTP/1.0 200 OK\r\n");
        if (content_type)
            fprintf(fp, "Content-Type: %s; charset=UTF-8\r\n", content_type);
    }
}


void cannot_do(int fd)
{
    FILE * fp = fdopen(fd,"w");

    fprintf(fp,"HTTP/1.0 501 Not Implemented\r\n");
    fprintf(fp,"Content-type:text/plain\r\n");

    char *response_body = "That command is not yet implemented\r\n";
    int content_length = strlen(response_body);
    fprintf(fp,"Content-Length: %d\r\n", content_length);
    fprintf(fp,"\r\n");
    fprintf(fp,"%s", response_body);
    fflush(fp);                  
    fclose(fp);
    exit(0);
}

void do_404(char *item, int fd)
{
    FILE *fp = fdopen(fd, "w");

    char response_body[BUFSIZ];
    sprintf(response_body, "The item you requested: %s\r\nis not found\r\n", item);

    int content_length = strlen(response_body);

    fprintf(fp, "HTTP/1.0 404 Not Found\r\n");
    fprintf(fp, "Content-Type: text/plain\r\n");
    fprintf(fp, "Content-Length: %d\r\n", content_length);
    fprintf(fp, "\r\n");

    fprintf(fp, "%s", response_body);
    fflush(fp);
    fclose(fp);
    exit(0);
}


int not_exist(char *f)
{
    struct stat info;
    return (stat(f,&info) == -1);
}

char * file_type(char *f)
{
    char * cp;
    if((cp = strrchr(f,'.')) != NULL)
        return cp + 1;
    return "";
}

void do_cat(char *f,int fd)
{
    char * extension = file_type(f);
    char * content = "text/plain";
    FILE * fpsock,*fpfile;
    int c;

    if(strcmp(extension,"html") == 0)
        content = "text/html";
    else if(strcmp(extension,"gif") == 0)
        content = "image/gif";
    else if(strcmp(extension,"jpg") == 0)
        content = "image/jpeg";
    else if(strcmp(extension,"jpeg") == 0)
        content = "image/jpeg";

    long file_size = 0;
    fpsock = fdopen(fd,"w");
    fpfile = fopen(f,"r");
    if (fpfile != NULL) {
        fseek(fpfile, 0, SEEK_END);
        file_size = ftell(fpfile);
        rewind(fpfile);
    }

    if(fpsock != NULL && fpfile != NULL)
    {
        header(fpsock,content);
        fprintf(fpsock, "Content-Length: %ld\r\n", file_size);
        fprintf(fpsock,"\r\n");
    while (1) {
        c = getc(fpfile);
        if (c == EOF) {
            if (feof(fpfile)) {
                // 到达文件末尾
                break;
            }
            if (ferror(fpfile)) {
                // 发生读取错误
                perror("Error reading file");
                break;
            }
        }
        putc(c, fpsock);
    }
        fclose(fpfile);
        fclose(fpsock);
    }
    exit(0);
}

int make_server_socket(int portnum)
{
    return make_server_socket_q(portnum,BACKLOG);
}

int make_server_socket_q(int portnum,int backlog)
{
    struct sockaddr_in saddr;
    int sock_id;

    sock_id = socket(PF_INET,SOCK_STREAM,0);
    if(sock_id == -1)
        return -1;
    
    bzero((void *)&saddr,sizeof(saddr));

    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(portnum);
    saddr.sin_family = AF_INET;
    if(bind(sock_id,(struct sockaddr *)&saddr,sizeof(saddr)) != 0)
        return -1;
    
    if(listen(sock_id,backlog) != 0)
        return -1;
    return sock_id;
}