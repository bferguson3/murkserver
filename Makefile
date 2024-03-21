# Makefile for Mac

INC = -I/Users/bent/Downloads/enet/include -I/opt/homebrew/opt/sqlite/include
LIB = -L/usr/local/lib -L/Users/bent/Downloads/sqlite-3450100/ -L/Users/bent/Downloads/enet/build

JSONJ = $(wildcard ./blobs/*.json)
JSONH = $(patsubst ./blobs/%.json, ./blobs/include/%_json.h, $(JSONJ))

default: $(JSONH)
	python3 tools/makejsonres.py blobs/include 
	$(shell itemizejson.sh)
	gcc client.c sha3.c base64.c client_packets.c -lenet -lsqlite3 $(LIB) $(INC) -o client -Wl,-rpath,/usr/local/lib/,-rpath,.
	gcc server.c sha3.c guid.c base64.c server_packets.c server_sql.c -lenet -lsqlite3 $(LIB) $(INC) -o server -Wl,-rpath,/usr/local/lib/,-rpath,.
    
blobs/include/%_json.h: blobs/%.json 
	python3 tools/json2cstr.py $< > $@
