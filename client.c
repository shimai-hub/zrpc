

#include <stdio.h>

#include "zrpc.h"
#include "nty_coroutine.h"
#include "cJSON.h"


int add(int a, int b);
char* cat(char* a, char* b, char* c);
char* sayhello(char* msg, int length);

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
    cJSON_Delete(root);

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

    free(body);
    free(char_return);
    cJSON_Delete(json_return);                                                                                                                                                            

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
    cJSON_Delete(root);
    
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
    cJSON_Delete(json_return);
    free(char_return);
    free(body);

    return char_result;

}

char* sayhello(char* msg, int length){

}

int main(){

    int a = 3,b = 4;
    int sum = add(a, b);
    printf("sum = %d\n",sum);

    // char* a = "abc";
    // char* b = "rpq";
    // char* c = "xyz";
    // char* ret = zcat(a, b, c);
    // printf("ret = %s\n",ret);
    return 0;
}