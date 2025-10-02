
CC = gcc
CFLAGS_COMMON = -Wall -Wextra
CFLAGS_GTK = `pkg-config --cflags --libs gtk+-3.0`

TARGET_CLI = serial-send
TARGET_GUI = serial-send-ui
SOURCE_CLI = serial_send.c
SOURCE_GUI = serial-send-ui.c

all: $(TARGET_CLI) $(TARGET_GUI)

$(TARGET_CLI): $(SOURCE_CLI)
	$(CC) -o $(TARGET_CLI) $(SOURCE_CLI) $(CFLAGS_COMMON)

$(TARGET_GUI): $(SOURCE_GUI)
	$(CC) -o $(TARGET_GUI) $(SOURCE_GUI) $(CFLAGS_COMMON) $(CFLAGS_GTK)

clean:
	rm -f $(TARGET_CLI) $(TARGET_GUI) $(TARGET_CLI)_debug $(TARGET_GUI)_debug

run-cli: $(TARGET_CLI)
	./$(TARGET_CLI)

run-gui: $(TARGET_GUI)
	./$(TARGET_GUI)

debug-cli: $(SOURCE_CLI)
	$(CC) -g -o $(TARGET_CLI)_debug $(SOURCE_CLI) $(CFLAGS_COMMON)

debug-gui: $(SOURCE_GUI)
	$(CC) -g -o $(TARGET_GUI)_debug $(SOURCE_GUI) $(CFLAGS_COMMON) $(CFLAGS_GTK)

.PHONY: all clean run-cli run-gui debug-cli debug-gui
