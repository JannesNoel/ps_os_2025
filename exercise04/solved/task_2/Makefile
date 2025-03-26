CFLAGS = -Wall -Wextra -std=c11

EXECUTABLES = cat_cut cat_grep_cut

.PHONY: all
all: $(EXECUTABLES)

.PHONY: clean
clean:
	$(RM) $(EXECUTABLES) *.o

cat_cut: cat_cut.o
	$(CC) $^ -o $@

cat_grep_cut: cat_grep_cut.o
	$(CC) $^ -o $@

cat_cut.o: cat_cut.c
	$(CC) $(CFLAGS) -c $<

cat_grep_cut.o: cat_grep_cut.c
	$(CC) $(CFLAGS) -c $<