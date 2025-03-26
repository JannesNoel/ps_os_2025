CFLAGS = -Wall -Wextra -std=c11

EXECUTABLES = server client

.PHONY: all
all: $(EXECUTABLES)

.PHONY: clean
clean:
	$(RM) $(EXECUTABLES) *.o

server: server.o
	$(CC) $^ -o $@

client: client.o
	$(CC) $^ -o $@

server.o: server.c
	$(CC) $(CFLAGS) -c $<

client.o: client.c
	$(CC) $(CFLAGS) -c $<