CFLAGS = -g -Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -pedantic-errors
INCLUDES_PATH = includes
SRC_PATH = src

main: $(SRC_PATH)/*.c $(INCLUDES_PATH)/*.h
	clang $(CFLAGS) -o bin/elp -I $(INCLUDES_PATH) -lm $(SRC_PATH)/*.c

sane: $(SRC_PATH)/*.c $(INCLUDES_PATH)/*.h
	clang $(CFLAGS) -fsanitize=address,undefined -o bin/elp -I $(INCLUDES_PATH) -lm $(SRC_PATH)/*.c

PHONY: clean
clean:
	rm obj/*.o bin/elp
