# Makefile for Mac

INC = -I/Users/bent/Downloads/enet-1.3.17/include -I/opt/homebrew/opt/sqlite/include

JSONJ = $(wildcard ./blobs/*.json)
JSONH = $(patsubst ./blobs/%.json, ./blobs/include/%_json.h, $(JSONJ))

default: $(JSONH)
	python3 tools/makejsonres.py blobs/include 
	$(shell itemizejson.sh)
	gcc client.c sha3.c base64.c -lenet -lsqlite3 -L/opt/homebrew/opt/sqlite/lib -L/usr/local/lib/ $(INC) -o client
	gcc server.c sha3.c guid.c base64.c -lenet -lsqlite3 -L/usr/local/lib -L/opt/homebrew/opt/sqlite/lib $(INC) -o server
    
blobs/include/%_json.h: blobs/%.json 
	python3 tools/json2cstr.py $< > $@