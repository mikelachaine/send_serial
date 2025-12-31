
CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -Wextra
LIBS = `pkg-config --libs gtk+-3.0`

TARGET = serial-send-ui
SOURCES = serial-send-ui.c serial-terminal-resources.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

serial-terminal-resources.c serial-terminal-resources.h: serial-terminal.gresource.xml serial-terminal.glade
	glib-compile-resources serial-terminal.gresource.xml --target=serial-terminal-resources.c --generate-source
	glib-compile-resources serial-terminal.gresource.xml --target=serial-terminal-resources.h --generate-header

serial-send-ui.o: serial-send-ui.c serial-terminal-resources.h
	$(CC) $(CFLAGS) -c serial-send-ui.c -o $@

serial-terminal-resources.o: serial-terminal-resources.c
	$(CC) $(CFLAGS) -c serial-terminal-resources.c -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) serial-terminal-resources.c serial-terminal-resources.h

.PHONY: all clean
