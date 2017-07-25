#gcc paramaters
CPPFLAGS+=-g -O -Wall -w -lpthread

#third library
HEADER_PATH = -I./include/
LIB_PATH = -L./lib/
LIBS = -ludt

all: server client 

clean:
	rm *.o server client; rm -rf /home/hly2070/share/server

server: server.o stun.o udp.o nettool.o
	$(CXX) -o $@  $^ $(LIB_PATH) $(LIBS) $(CPPFLAGS)

client: client.o stun.o udp.o
	$(CXX) -o $@  $^ $(LIB_PATH) $(LIBS) $(CPPFLAGS)

%.o:%.cpp
	$(CXX)  -c $< -o $@ $(HEADER_PATH) $(CPPFLAGS)

%:RCS/%
	co $@

rel:
	cp server /home/hly2070/share/

# Dependancies
server.o: ./include/*.h stun.h nettool.h net_def.h
client.o: ./include/*.h stun.h
stun.o: stun.h udp.h
udp.o: stun.h udp.h
