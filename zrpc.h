

#ifndef __ZRPC_H__
#define __ZRPC_H__



#define ZRPC_MESSAGE_HEADER_LENGTH  8
#define ZRPC_MESSAGE_HEADER_VERSION "01"

#define ZRPC_CRC32_TABLE_LENGTH     156


static unsigned int crc32_table[ZRPC_CRC32_TABLE_LENGTH] = {0};



int zrpc_connect_server(char* ip,unsigned short port);
void rpc_session(char* body);





#endif

