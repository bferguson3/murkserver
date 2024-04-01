INC:=-I/home/bent/Downloads/enet-1.3.17/include/ -I/home/bent/Downloads/sqlite-autoconf-3450200/
#LIB:=/usr/lib64/ 
LIB:=/usr/local/lib/

default:
	g++ $(INC) -L$(LIB) -lenet -lsqlite3 main.cpp server.cpp user.cpp ../cppclient/packet.cpp guid.cpp -o server -Wl,-rpath,.
