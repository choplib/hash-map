CC = clang

TARGET = bin/hashmap

SRC = src/main.c \
      src/hashmap/hashmap.c \
      external/dynamic-array/src/dynamic_array/array.c

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic \
         -g -O0 \
         -fsanitize=address,undefined \
         -I./src \
         -I./external/dynamic-array/src

LDFLAGS = -fsanitize=address,undefined

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf bin

