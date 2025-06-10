CFLAGS = -g -Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -pedantic-errors
INCLUDES_PATH = includes
SRC_PATH = src

main: $(SRC_PATH)/*.c $(INCLUDES_PATH)/*.h
	clang $(CFLAGS) -o bin/elp -I $(INCLUDES_PATH) $(SRC_PATH)/*.c

obj/f_manip.o: src/f_manip.c includes/f_manip.h
	clang $(CFLAGS) $(INCLUDES) src/f_manip.c -o obj/f_manip.o

PHONY: clean
clean:
	rm obj/*.o bin/elp
