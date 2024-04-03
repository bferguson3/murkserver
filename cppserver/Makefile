INC:=-I/home/bent/Downloads/enet-1.3.17/include/ -I/home/bent/Downloads/sqlite-autoconf-3450200/ -I../ -I../cppclient/
#LIB:=/usr/lib64/ 
LIB:=/usr/local/lib/

SRC:=main.cpp server.cpp user.cpp ../cppclient/packet.cpp guid.cpp screen.cpp

default:
	g++ $(INC) -L$(LIB) $(SRC) -lenet -lsqlite3  -o server -Wl,-rpath,. -Wall -Werror
