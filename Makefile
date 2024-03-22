# Makefile for Mac

INC = -I/Users/bent/Downloads/enet/include -I/opt/homebrew/opt/sqlite/include
LIB = -L/usr/local/lib -L/Users/bent/Downloads/sqlite-3450100/ -L/Users/bent/Downloads/enet/build

JSONJ = $(wildcard ./blobs/*.json)
JSONH = $(patsubst ./blobs/%.json, ./blobs/include/%_json.h, $(JSONJ))

CLIENT_SRC = client.c res/sha3.c res/base64.c client_packets.c 
SERVER_SRC = server.c res/sha3.c res/guid.c res/base64.c server_packets.c server_sql.c 

default: $(JSONH)
	python3 tools/makejsonres.py blobs/include 
	$(shell itemizejson.sh)
	gcc $(CLIENT_SRC) -lenet $(LIB) $(INC) -o client -Wl,-rpath,/usr/local/lib/,-rpath,.
	gcc $(SERVER_SRC) -lenet -lsqlite3 $(LIB) $(INC) -o server -Wl,-rpath,/usr/local/lib/,-rpath,.
    
blobs/include/%_json.h: blobs/%.json 
	python3 tools/json2cstr.py $< > $@

.PHONY: docs 

docs:
	rm -rf .docs 
	rm -rf docs 
	doxygen doxygen.cfg 
	mv ./.docs/html ./docs 