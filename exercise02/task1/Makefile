CFLAGS = -std=c11 -Wall -Wextra

.PHONY: all
all: main

.PHONY: clean
clean:
	$(RM) key.c *.o main

.PHONY: create_key
create_key: announce_"Creating\ key.c"
	@touch key.c
	@echo \
	"#include \"key.h\"\n\n\
	const char* get_key() {\n\
	\treturn \"$$(whoami)\";\n\
	}" > key.c

.PHONY: announce_%
announce_%:
	@echo $*

main: main.o key.o | announce_"Linking\ main"
	$(CC) $^ -o $@

main.o: main.c key.h | announce_"Compiling\ main.c"
	$(CC) $(CFLAGS) -c $<

key.o: key.h | create_key announce_"Compiling\ key.c"
	$(CC) $(CFLAGS) -c key.c
