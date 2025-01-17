


CC = gcc

TARGET = server client

SRCS_SERVER = server.c zrpc.c 
SRCS_CLIENT = client.c zrpc.c 

SRCS_CJSON = ../cJSON/cJSON.c


OBJS_CJSON = $(SRCS_CJSON:.c=.o)
OBJS_SERVER = $(SRCS_SERVER:.c=.o)
OBJS_CLIENT = $(SRCS_CLIENT:.c=.o)

CJSON_INC = -I ../cJSON/
INC = -I ../NtyCo/core 

LIB = -L ../NtyCo -lntyco

all:$(TARGET)

server:$(OBJS_SERVER) $(OBJS_CJSON)
	$(CC) -o $@ $^ $(LIB)

client:$(OBJS_CLIENT) $(OBJS_CJSON)
	$(CC) -o $@ $^ $(LIB)

$(OBJS_CJSON):$(SRCS_CJSON)
	$(CC) -c $^ -o $@ $(CJSON_INC)

%.o:%.c
	$(CC) -c $^ -o $@ $(INC) $(CJSON_INC)

clean:
	rm -f $(OBJS_CLIENT) $(OBJS_SERVER) $(TARGET)