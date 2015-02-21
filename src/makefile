all: httpserver

httpserver: httpserver.c
	clang -g -lpthread server_helper.c httpserver.c -o httpserver

clean:
	rm -rf *.o

clobber: clean
	rm -rf httpserver