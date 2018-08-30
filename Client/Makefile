CC=gcc
INCLUDE_DIR=../include

all:
	$(CC) -Wall -o userApp imgNegCli.c -I$(INCLUDE_DIR) -lm -lpthread

all_profiled:
	$(CC) -DPROFILING -Wall -o userApp imgNegCli.c -I$(INCLUDE_DIR) -lm -lpthread
	
clean:
	rm userApp
	
.SILENT:
