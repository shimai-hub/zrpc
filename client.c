

#include <stdio.h>

#include "zrpc.h"
#include "nty_coroutine.h"
#include "cJSON.h"


extern int add(int a, int b);
extern char* cat(char* a, char* b, char* c);
extern char* sayhello(char* msg, int length);

int main(int argc, char* argv[]){

    printf("%s:\n%s\n", argv[1], read_conf(argv[1]));

    // int a = 3,b = 4;
    // int sum = add(a, b);
    // printf("sum = %d\n",sum);

    // char* a = "abc ";
    // char* b = "rpq ";
    // char* c = "xyz";
    // char* ret = zcat(a, b, c);
    // printf("ret = %s\n",ret);
    // free(ret);

    // char* msg = "abcdefg";
    // msg = sayhello(msg, strlen(msg));
    // printf("msg : %s\n", msg);
    // free(msg);

    return 0;
}