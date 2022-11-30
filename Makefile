
CC = g++ -Wno-write-strings
SERVER_FILE = simple_server.cpp
HTTP_SERVER_FILE = http_server.cpp
LOAD_GEN_FILE = load_gen.c

all: server load_gen

server: $(SERVER_FILE) $(HTTP_SERVER_FILE)
	$(CC) $(SERVER_FILE) $(HTTP_SERVER_FILE) -o server

load_gen : load_gen.c
	gcc -g load_gen.c -pthread -o load_gen

clean:
	rm -f server
