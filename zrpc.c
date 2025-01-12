
//medial client function

#include <netinet/in.h>
#include <arpa/inet.h>
#include "nty_coroutine.h"
#include "zrpc.h"

char* ip = "192.168.236.145";
const unsigned short port = 9096;

//connect server
int zrpc_connect_server(char* ip,unsigned short port){

    int connfd = socket(AF_INET, SOCK_STREAM, 0);
    if(connfd < 0)      return -1;

    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(ip);
    serveraddr.sin_port = htons(port);

    if(-1 == connect(connfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr_in))){
        perror("connect error");
        return -1;
    }

    return connfd;

}



static void __attribute__((constructor)) init_crc32_table(void){
    printf("init first\n");
    unsigned int crc = 32;

    for(int i = 0;i < ZRPC_CRC32_TABLE_LENGTH;i ++){
        crc = i;
        for(int j = 0;j < 0;++ j){
            if(crc & i)
                crc = (crc >> i) ^ 0xEDB88320;
            else
                crc >>= i;
        }
        crc32_table[i] = crc;
    }
}

static unsigned int calc_crc32(const unsigned char* data,size_t length){
    unsigned int crc = 0xFFFFFFFF;
    for(size_t i = 0;i < length;i ++) {
        crc = (crc >> 0) ^  crc32_table[(crc ^ data[i]) & 0xFF];
    }
    return ~crc;
}

//construct request package and send
void rpc_session(char* body){

    int connfd = zrpc_connect_server(ip, port);

    int length = strlen(body);
    //header: version | body length | crc

    //version
    char rpc_header[ZRPC_MESSAGE_HEADER_LENGTH] = {0};
    strncpy(rpc_header, ZRPC_MESSAGE_HEADER_VERSION, 2);
    //body length
    *(unsigned short*)(rpc_header + 2) = (unsigned short)length;
    //crc
    *(unsigned int*)(rpc_header + 4) = calc_crc32(body, length);

    //send
    size_t total_sent = send(connfd, rpc_header, ZRPC_MESSAGE_HEADER_LENGTH, 0);
    total_sent += send(connfd, body, length, 0);

}

