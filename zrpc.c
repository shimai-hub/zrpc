
//medial client function

#include <netinet/in.h>
#include <arpa/inet.h>
#include "nty_coroutine.h"
#include "zrpc.h"
#include "cJSON.h"

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

//construct zrpc_header
char* zrpc_header_encode(char* rpc_header, char* body){

    //header: version | body length | crc

    //version
    strncpy(rpc_header, ZRPC_MESSAGE_HEADER_VERSION, 2);
    //body length
    *(unsigned short*)(rpc_header + 2) = (unsigned short)strlen(body);
    //crc
    *(unsigned int*)(rpc_header + 4) = calc_crc32(body, strlen(body));

    return rpc_header;
}

//client send and recv
char* zrpc_client_session(char* body){

    int connfd = zrpc_connect_server(ip, port);

    int length = strlen(body);
//send
    //header
    char rpc_header[ZRPC_MESSAGE_HEADER_LENGTH] = {0};
    zrpc_header_encode(rpc_header, body);

    //send
    size_t total_sent = send(connfd, rpc_header, ZRPC_MESSAGE_HEADER_LENGTH, 0);
    total_sent += send(connfd, body, length, 0);

//recv and parser

    //header
    int ret = 0;
    memset(rpc_header, 0, ZRPC_MESSAGE_HEADER_LENGTH);
    ret = recv(connfd, rpc_header, ZRPC_MESSAGE_HEADER_LENGTH, 0);

    assert(ret == ZRPC_MESSAGE_HEADER_LENGTH);
    
    //body
    unsigned short r_length = 0;
    r_length = *(unsigned short*)(rpc_header + 2);

    char* payload = (char*)malloc(r_length + 1);
    assert(payload != 0);
    memset(payload, 0 , r_length + 1);

    ret = recv(connfd, payload, r_length, 0);
    assert(ret == r_length);
    
    printf("response: %s\n",payload);
    // free(payload);

    close(connfd);

    return payload;
}

int zrpc_method_add(int a, int b){
    return a + b;
}

char* zrpc_method_zcat(char* a, char* b, char* c){
    
}

//server recv and send
char* zrpc_server_session(char* payload){

    cJSON* root = cJSON_Parse(payload);
    if(root == NULL)    return NULL;

    cJSON* method = cJSON_GetObjectItem(root, "method");
    cJSON* params = cJSON_GetObjectItem(root, "params");
    cJSON* callerid = cJSON_GetObjectItem(root, "callerid");

    if(strcmp(method->valuestring, "add") == 0){
        
        cJSON* a = cJSON_GetObjectItem(params, "a");
        cJSON* b = cJSON_GetObjectItem(params, "b");
        int result = zrpc_method_add(a->valueint, b->valueint);

        cJSON* json_response = cJSON_CreateObject();
        cJSON_AddStringToObject(json_response, "method", "add");
        cJSON_AddNumberToObject(json_response, "result", result);
        cJSON_AddNumberToObject(json_response, "callerid", callerid->valueint);

        char* char_response = cJSON_Print(json_response);

        cJSON_Delete(root);
        cJSON_Delete(json_response);

        return char_response;

    }else if(strcmp(method->valuestring, "zcat") == 0){

    }else if(strcmp(method->valuestring, "sayhello") == 0){
        
    }

}