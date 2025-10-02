
CC = gcc
CFLAGS_COMMON = -Wall -Wextra
CFLAGS_GTK = `pkg-config --cflags --libs gtk+-3.0`

TARGET_CLI = serial-send
TARGET_GUI = serial-send-ui
TARGET_RADIO = radio-ui
SOURCE_CLI = serial_send.c
SOURCE_GUI = serial-send-ui.c
SOURCE_RADIO = radio-ui.c

# CAT library objects
CAT_OBJS = radios/ftx1_cat.o

all: $(TARGET_CLI) $(TARGET_GUI) $(TARGET_RADIO)

$(TARGET_CLI): $(SOURCE_CLI)
	$(CC) -o $(TARGET_CLI) $(SOURCE_CLI) $(CFLAGS_COMMON)

$(TARGET_GUI): $(SOURCE_GUI)
	$(CC) -o $(TARGET_GUI) $(SOURCE_GUI) $(CFLAGS_COMMON) $(CFLAGS_GTK)

$(TARGET_RADIO): $(SOURCE_RADIO) $(CAT_OBJS)
	$(CC) -o $(TARGET_RADIO) $(SOURCE_RADIO) $(CAT_OBJS) $(CFLAGS_COMMON) $(CFLAGS_GTK)

# CAT library compilation
radios/ftx1_cat.o: radios/ftx1_cat.c radios/ftx1_cat.h
	$(CC) -c -o $@ $< $(CFLAGS_COMMON)

clean:
	rm -f $(TARGET_CLI) $(TARGET_GUI) $(TARGET_RADIO) $(TARGET_CLI)_debug $(TARGET_GUI)_debug $(TARGET_RADIO)_debug $(CAT_OBJS)

run-cli: $(TARGET_CLI)
	./$(TARGET_CLI)

run-gui: $(TARGET_GUI)
	./$(TARGET_GUI)

run-radio: $(TARGET_RADIO)
	./$(TARGET_RADIO)

debug-cli: $(SOURCE_CLI)
	$(CC) -g -o $(TARGET_CLI)_debug $(SOURCE_CLI) $(CFLAGS_COMMON)

debug-gui: $(SOURCE_GUI)
	$(CC) -g -o $(TARGET_GUI)_debug $(SOURCE_GUI) $(CFLAGS_COMMON) $(CFLAGS_GTK)

debug-radio: $(SOURCE_RADIO) $(CAT_OBJS)
	$(CC) -g -o $(TARGET_RADIO)_debug $(SOURCE_RADIO) $(CAT_OBJS) $(CFLAGS_COMMON) $(CFLAGS_GTK)

.PHONY: all clean run-cli run-gui run-radio debug-cli debug-gui debug-radio
