

#include <stdio.h>

#include "zrpc.h"
#include "nty_coroutine.h"


int add(int a,int b){
    char* addjson = "{           \
    \"method\": \"add\",         \
    \"params\": \"3\", \"4\",    \
    \"types\":  \"int\", \"int\",\
    \"rettype\":\"int\",         \
    \"callerid\": 1234568        \
    }";
    rpc_session(addjson);
}

int main(){

    int a = 3,b = 4;
    int sum = add(a, b);
    printf("sum = %d\n",sum);

    return 0;
}