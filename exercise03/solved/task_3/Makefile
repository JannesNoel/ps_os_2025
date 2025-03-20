CFLAGS = -std=c11 -Wall -Wextra -pthread

TARGET_A = task_3a
TARGET_B = task_3b

TARGETS = $(TARGET_A) $(TARGET_B)

.PHONY: all
all: task_3a task_3b

.PHONY: clean
clean:
	$(RM) $(TARGETS) *.o

task_3a.o: task_3a.c
	$(CC) $(CFLAGS) -c $^

task_3b.o: task_3b.c
	$(CC) $(CFLAGS) -c $^

task_3a: task_3a.o
	$(CC) $^ -o $@

task_3b: task_3b.o
	$(CC) $^ -o $@
