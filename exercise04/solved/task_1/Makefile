CFLAGS = -Wall -Wextra -std=c11
PROGRAM_NAME = arg_counter

.PHONY: all
all: $(PROGRAM_NAME)

.PHONY: clean
clean:
	$(RM) $(PROGRAM_NAME) *.o

arg_counter: arg_counter.o
	$(CC) $^ -o $@

arg_counter.o: arg_counter.c
	$(CC) $(CFLAGS) -c $<
