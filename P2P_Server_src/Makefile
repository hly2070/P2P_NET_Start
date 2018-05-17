CC = gcc
CPLUSPLUS = g++
OBJ = obj

#gcc paramaters
CCFLAGS = -lstdc++ -fPIC -g -Wall -O3 -w libsqlite3.a -lpthread -lm -ldl
CPPFLAGS = -lstdc++ -fPIC -g -O3 -Wall -w libsqlite3.a -lpthread -lm -ldl

#third library
INC = -I./
LIB_PATH =
LIBS =

OBJS = FTCTask.o FTCSock.o nettool.o FTCPeer.o FTCMutex.o FTCIcmp.o db_sqlite.o

all: server client
	chmod 777 server; chmod 777 client

.c.o:
	$(CC) -c $(INC) $(CFLAGS) $< -o $@

.cpp.o:
	$(CPLUSPLUS) -c $(INC) $(CPPFLAGS) $< -o $@
	
clean:
	rm -rf *.o server client

server: $(OBJS) server.o
	$(CPLUSPLUS) -g -o $@  $^ $(INC) $(CPPFLAGS) 

client: $(OBJS) client.o
	$(CPLUSPLUS) -g -o $@  $^ $(INC) $(CPPFLAGS)
