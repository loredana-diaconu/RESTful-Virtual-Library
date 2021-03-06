CC=g++
CFLAGS=-I.

client: client.cpp requests.cpp helpers.cpp buffer.cpp parson.c
	$(CC) -o client client.cpp requests.cpp helpers.cpp buffer.cpp parson.c -Wall

run: client
	./client

clean:
	rm -f *.o client
