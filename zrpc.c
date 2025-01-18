
//medial client function

#include <netinet/in.h>
#include <arpa/inet.h>
#include "nty_coroutine.h"
#include "zrpc.h"
#include "cJSON.h"

char* ip = "192.168.236.145";
const unsigned short port = 9096;

static int global_caller_id = 0x12345678;

int add(int a, int b){

    //construct json_body
    cJSON* root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "method", "add");

    cJSON* params = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "params", params);
    cJSON_AddNumberToObject(params, "a", a);
    cJSON_AddNumberToObject(params, "b", b);
    cJSON_AddNumberToObject(root, "callerid", global_caller_id++);

    //get char_body
    char* body = cJSON_Print(root);
    printf("body: %s\n",body);

    //send and get char_return
    char* char_return = zrpc_client_session(body);

    //get json_return
    cJSON* json_return = cJSON_Parse(char_return);

    //get json_result
    cJSON* json_result = cJSON_GetObjectItem(json_return, "result");

    //get char_result
    char* char_result = cJSON_Print(json_result);
    printf("response: %s\n",char_result);
    
    //free
    cJSON_Delete(root);
    cJSON_Delete(json_return);  
    free(body);
    free(char_return);
                                                                                                                                                              

    return atoi(char_result);

}

char* zcat(char* a, char* b, char* c){

    //construct json_body
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "method", "zcat");

    cJSON* params = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "params", params);

    cJSON_AddStringToObject(params, "a", a);
    cJSON_AddStringToObject(params, "b", b);
    cJSON_AddStringToObject(params, "c", c);

    cJSON_AddNumberToObject(root, "callerid", global_caller_id ++);
    
    //get char_body
    char* body = cJSON_Print(root);
    printf("body: %s\n",body);
    
    //send and get char_return
    char* char_return = zrpc_client_session(body);
    
    //get json_return
    cJSON* json_return = cJSON_Parse(char_return);
    
    //get json_result
    cJSON* json_result = cJSON_GetObjectItem(json_return, "result");

    //get char_result
    char* char_result = cJSON_Print(json_result);
    printf("response: %s\n",char_result);

    //free
    cJSON_Delete(root);
    cJSON_Delete(json_return);
    free(body);
    free(char_return);

    return char_result;

}

char* sayhello(char* msg, int length){
    
    cJSON* root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "method", "sayhello");

    cJSON* params = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "params", params);
    cJSON_AddStringToObject(params, "msg", msg);
    cJSON_AddNumberToObject(params, "length", length);

    cJSON_AddNumberToObject(root, "callerid", global_caller_id ++);

    char* body = cJSON_Print(root);

    char* char_return = zrpc_client_session(body);
    cJSON* json_return = cJSON_Parse(char_return);
    cJSON* json_result = cJSON_GetObjectItem(json_return, "result");
    char* char_result = cJSON_Print(json_result);

    cJSON_Delete(root);
    cJSON_Delete(json_return);
    free(body);
    free(char_return);

    return char_result;

}

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

char* read_conf(char* filename){
    
    int fd = open(filename, O_RDONLY);
    if(fd == -1){
        perror("open error");
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    char* buffer = (char*)malloc(file_size + 1);
    if(buffer == NULL){
        close(fd);
        return NULL;
    }
    memset(buffer, 0, file_size + 1);

    size_t byte_size = read(fd, buffer, file_size);
    buffer[byte_size] = '\0';

    close(fd);

    return buffer;

}

int zrpc_method_add(int a, int b){
    return a + b;
}

char* zrpc_method_zcat(char* a, char* b, char* c){
    
    if(a == NULL || b == NULL || c == NULL)     return NULL;

    char* ret = (char*)malloc(strlen(a) + strlen(b)+ strlen(c) + 1);
    memset(ret, 0 ,strlen(a) + strlen(b)+ strlen(c) + 1);

    memcpy(ret, a, strlen(a));
    memcpy(ret + strlen(a), b, strlen(b));
    memcpy(ret + strlen(a) + strlen(b), c, strlen(c));

    return ret;
}

char* zrpc_method_sayhello(char* msg, int length){

    if(msg == NULL)     return NULL;

    for(int i = 0;i < length/2;i ++){
        
        int tmp = msg[i];
        msg[i] = msg[length - i - 1];
        msg[length - i - 1] = tmp;
        
    }

    return msg;

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
        if(char_response == NULL)       return NULL;
        
        cJSON_Delete(root);
        cJSON_Delete(json_response);

        return char_response;

    }else if(strcmp(method->valuestring, "zcat") == 0){
        
        cJSON* a = cJSON_GetObjectItem(params, "a");
        cJSON* b = cJSON_GetObjectItem(params, "b");
        cJSON* c = cJSON_GetObjectItem(params, "c");

        char* result = zrpc_method_zcat(a->valuestring, b->valuestring, c->valuestring);
        if(result == NULL)      return NULL;

        cJSON* json_response = cJSON_CreateObject();
        cJSON_AddStringToObject(json_response, "method", "zcat");
        cJSON_AddStringToObject(json_response, "result", result);
        cJSON_AddNumberToObject(json_response, "callerid", callerid->valueint);

        char* char_response = cJSON_Print(json_response);

        cJSON_Delete(root);
        cJSON_Delete(json_response);
        free(result);

        return char_response;

    }else if(strcmp(method->valuestring, "sayhello") == 0){

        cJSON* msg = cJSON_GetObjectItem(params, "msg");
        cJSON* length = cJSON_GetObjectItem(params, "length");

        char* result = zrpc_method_sayhello(msg->valuestring, length->valueint);
        if(result == NULL)      return NULL;

        cJSON* json_response = cJSON_CreateObject();
        cJSON_AddStringToObject(json_response, "method", "sayhello");
        cJSON_AddStringToObject(json_response, "result", result);
        cJSON_AddNumberToObject(json_response, "callerid", callerid->valueint);

        char* char_response = cJSON_Print(json_response);

        cJSON_Delete(root);
        cJSON_Delete(json_response);

        return char_response;

    }

}