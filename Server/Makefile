CC=gcc
OUT_DIR=bin
SRC_DIR=src
INCLUDE_DIR=../include

all:
	mkdir -p $(OUT_DIR)
	$(CC) -Wall -o bin/imgNegServer $(SRC_DIR)/imgNegServer.c -I$(INCLUDE_DIR) -lm
    
run_server:
	bin/imgNegServer
	
clean:
	rm bin/*
	
.SILENT:
