INC:=-I/home/bent/Downloads/enet-1.3.17/include/ -I/home/bent/Downloads/sqlite-autoconf-3450200/
LIB:=/usr/lib64/ #/home/bent/Downloads/enet-1.3.17/build/

SRC:=main.cpp client.cpp getc.cpp packet.cpp screen.cpp # ../res/base64.c ../res/sha3.c

default:
	g++ $(INC) -L$(LIB) -lenet $(SRC) -o client -Wl,-rpath,.
