

#include <unistd.h>
#include <netinet/in.h>
#include "nty_coroutine.h"
#include "zrpc.h"

#define MAX_BUFFER_LENGTH   1024


void zrpc_handle(void* arg){
    
    int remotefd = *(int*)arg;
    free(arg);

    while(1){

        char rpc_header[ZRPC_MESSAGE_HEADER_LENGTH] = {0};
        int ret = recv(remotefd, rpc_header, ZRPC_MESSAGE_HEADER_LENGTH, 0);

        if(ret == ZRPC_MESSAGE_HEADER_LENGTH){

            size_t length = *(unsigned short*)(rpc_header + 2);

            char payload[MAX_BUFFER_LENGTH] = {0};
            recv(remotefd, payload, length, 0);
            char* char_response = zrpc_server_session(payload);
            
            memset(rpc_header, 0 ,ZRPC_MESSAGE_HEADER_LENGTH);
            zrpc_header_encode(rpc_header, char_response);

            send(remotefd, rpc_header, ZRPC_MESSAGE_HEADER_LENGTH, 0);      
            send(remotefd, char_response, strlen(char_response), 0);

            close(remotefd);
            
        }else if(ret == 0){
            close(remotefd);
            break;
        }else if(ret < 0){
            close(remotefd);
            break;
        }else{
            printf("rpc header error\n");
            close(remotefd);
            break;
        }

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