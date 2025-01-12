

#include <unistd.h>
#include <netinet/in.h>
#include "nty_coroutine.h"

#define MAX_BUFFER_LENGTH   1024

void zrpc_handle(void* arg){
    
    int remotefd = *(int*)arg;
    free(arg);

    while(1){

        char buffer[MAX_BUFFER_LENGTH] = {0};
        int ret = recv(remotefd, buffer, MAX_BUFFER_LENGTH, 0);
        if(ret > 0){
            printf("recv: %s\n",buffer + 8);

        }else if(ret == 0){
            close(remotefd);
            break;
        }else if(ret < 0){
            close(remotefd);
            break;
        }

        ret = send(remotefd, buffer, strlen(buffer), 0);

    }

}

void zrpc_listen(void* arg){
    
    unsigned short port = *(unsigned short*)arg;

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)    return ;

    struct sockaddr_in local, remote;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(port);

    if(-1 == bind(listenfd, (struct sockaddr*)&local, sizeof(struct sockaddr_in))){
        perror("bind error");
    }

    if(-1 == listen(listenfd, 20)){
        perror("listen error");
    }

    while(1){

        socklen_t len = sizeof(struct sockaddr_in);

        int remotefd = accept(listenfd, (struct sockaddr*)&remote, &len);
        if(remotefd < 0)    continue;

        int* arg = (int*)malloc(sizeof(int));
        *arg = remotefd;
        
        nty_coroutine *co = NULL;
        nty_coroutine_create(&co, zrpc_handle, arg);

    }

}


int main(){
    
    unsigned short port = 9096;

    nty_coroutine *co = NULL;
    nty_coroutine_create(&co, zrpc_listen, &port);
    nty_schedule_run();
    
    return 0;
}