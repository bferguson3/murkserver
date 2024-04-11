INC:=-I~/Downloads/enet-1.3.17/include/ -I~/Downloads/sqlite-autoconf-3450200/ -I../cppserver/
LIB:=/usr/lib64/ #~/Downloads/enet-1.3.17/build/

SRC:=main.cpp \
	client.cpp \
	getc.cpp \
	packet.cpp \
	../cppserver/screen.cpp # ../res/base64.c ../res/sha3.c

default:
	g++ $(INC) -L$(LIB) -lenet $(SRC) -o client -Wl,-rpath,. -Wall -Werror 
