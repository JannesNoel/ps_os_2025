CFLAGS = -Wall -Wextra -std=c11

.PHONY: all
all: signal_handler

.PHONY: clean
clean:
	$(RM) *.o signal_handler

signal_handler: signal_handler.o
	$(CC) $^ -o $@

signal_handler.o: signal_handler.c
	$(CC) $(CFLAGS) -D_DEFAULT_SOURCE -c $<
