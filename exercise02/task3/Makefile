CLFAGS= -std=c11 -Wall -Wextra -Wpedantic

.PHONY: all
all: task3

.PHONY: clean
clean: 
	$(RM) *.o task3

task3: task3.o
	$(CC) $^ -o $@

task3.o: task3.c
	$(CC) $(CFLAGS) -c $^