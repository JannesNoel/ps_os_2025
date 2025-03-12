CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -fsanitize=address

.PHONY: clean
clean:
	$(RM) *.pbm *.o

.PHONY: cleanAll
cleanAll:
	$(RM) game *.pbm *.gif *.o

.PHONY: convert
convert:
	magick -delay 10 gol_*.pbm -filter point -resize 300% gol.gif


game: game.c
	$(CC) $(CFLAGS) $^ -o $@
