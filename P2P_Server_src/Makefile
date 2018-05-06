CC = gcc
CPLUSPLUS = g++
OBJ = obj

#gcc paramaters
CCFLAGS = -fPIC -g -Wall -O3 -w -lpthread
CPPFLAGS = -fPIC -g -O3 -Wall -w -lpthread

#third library
INC = -I./
LIB_PATH =
LIBS =

OBJS = FTCTask.o FTCSock.o nettool.o FTCPeer.o FTCMutex.o

all: server client
	chmod 777 server; chmod 777 client

.c.o:
	$(CC) -c $(INC) $(CFLAGS) $< -o $@

.cpp.o:
	$(CPLUSPLUS) -c $(INC) $(CPPFLAGS) $< -o $@
	
clean:
	rm -rf *.o server client

server: $(OBJS) server.o
	$(CPLUSPLUS) -o $@  $^ $(INC) $(CPPFLAGS)

client: $(OBJS) client.o
	$(CPLUSPLUS) -o $@  $^ $(INC) $(CPPFLAGS)