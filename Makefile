
CC = gcc
CFLAGS = `pkg-config --cflags --libs gtk+-3.0` -Wall -Wextra
TARGET = cat_test
SOURCE = cat_test.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) -o $(TARGET) $(SOURCE) $(CFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

debug: $(SOURCE)
	$(CC) -g -o $(TARGET)_debug $(SOURCE) $(CFLAGS)

.PHONY: all clean run debug
