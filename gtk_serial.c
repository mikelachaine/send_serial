#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <glib.h>
#include <sys/types.h>

// Serial communication structures and functions
typedef struct {
    int fd;
    gboolean connected;
    GtkBuilder *builder;
    GtkWidget *main_window;
    GtkWidget *device_entry;
    GtkWidget *baud_combo;
    GtkWidget *connect_button;
    GtkWidget *disconnect_button;
    GtkWidget *status_label;
    GtkWidget *command_entry;
    GtkWidget *send_button;
    GtkWidget *response_textview;
    GtkTextBuffer *response_buffer;
    guint read_source_id;
} AppData;

// Baud rate table
static const struct {
    int baud;
    speed_t constant;
} baud_table[] = {
    { 1200,   B1200   },
    { 2400,   B2400   },
    { 4800,   B4800   },
    { 9600,   B9600   },
    { 19200,  B19200  },
    { 38400,  B38400  },
    { 57600,  B57600  },
    { 115200, B115200 },
    { 230400, B230400 },
    { 460800, B460800 },
    { 921600, B921600 }
};
#define BAUD_TABLE_SIZE (sizeof(baud_table)/sizeof(baud_table[0]))

static speed_t baud_to_constant(int baud) {
    for (size_t i = 0; i < BAUD_TABLE_SIZE; ++i) {
        if (baud_table[i].baud == baud) {
            return baud_table[i].constant;
        }
    }
    return B38400; // fallback
}

static int init_serial(const char *device, int baudrate) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return -1;
    }

    cfsetospeed(&tty, baud_to_constant(baudrate));
    cfsetispeed(&tty, baud_to_constant(baudrate));

    tty.c_cflag &= ~PARENB; // No parity
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    tty.c_cflag &= ~CSIZE;  // Clear data size bits
    tty.c_cflag |= CS8;     // 8 data bits
    tty.c_cflag &= ~CRTSCTS; // No hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem control lines

    tty.c_lflag &= ~ICANON; // Non-canonical mode
    tty.c_lflag &= ~ECHO;   // No echo
    tty.c_lflag &= ~ECHOE;  // No echo erase
    tty.c_lflag &= ~ISIG;   // No signal chars

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // No software flow control
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

    tty.c_oflag &= ~OPOST; // No output processing
    tty.c_oflag &= ~ONLCR; // No \n -> \r\n conversion

    tty.c_cc[VTIME] = 1;    // Wait up to 0.1 second
    tty.c_cc[VMIN] = 0;     // Return immediately with what we have

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return -1;
    }

    return fd;
}

static void append_to_response(AppData *app_data, const char *text) {
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(app_data->response_buffer, &iter);
    
    // Add timestamp
    GDateTime *now = g_date_time_new_now_local();
    gchar *timestamp = g_date_time_format(now, "[%H:%M:%S] ");
    gtk_text_buffer_insert(app_data->response_buffer, &iter, timestamp, -1);
    g_free(timestamp);
    g_date_time_unref(now);
    
    // Add the actual text
    gtk_text_buffer_insert(app_data->response_buffer, &iter, text, -1);
    gtk_text_buffer_insert(app_data->response_buffer, &iter, "\n", -1);
    
    // Auto-scroll to bottom
    GtkTextMark *mark = gtk_text_buffer_get_insert(app_data->response_buffer);
    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(app_data->response_textview), mark);
}

static gboolean serial_read_callback(GIOChannel *source, GIOCondition condition, gpointer data) {
    AppData *app_data = (AppData *)data;
    char buffer[256];
    ssize_t bytes_read;

    if (condition & G_IO_HUP) {
        append_to_response(app_data, "Connection lost");
        app_data->connected = FALSE;
        gtk_label_set_text(GTK_LABEL(app_data->status_label), "Disconnected");
        gtk_widget_set_sensitive(app_data->connect_button, TRUE);
        gtk_widget_set_sensitive(app_data->disconnect_button, FALSE);
        gtk_widget_set_sensitive(app_data->command_entry, FALSE);
        gtk_widget_set_sensitive(app_data->send_button, FALSE);
        return FALSE;
    }

    if (condition & G_IO_IN) {
        bytes_read = read(app_data->fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            // Remove trailing newlines for cleaner display
            while (bytes_read > 0 && (buffer[bytes_read-1] == '\n' || buffer[bytes_read-1] == '\r')) {
                buffer[--bytes_read] = '\0';
            }
            if (bytes_read > 0) {
                gchar *recv_msg = g_strdup_printf("RECV: %s", buffer);
                append_to_response(app_data, recv_msg);
                g_free(recv_msg);
            }
        } else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("read");
            return FALSE;
        }
    }

    return TRUE;
}

// Signal handlers - these names must match the Glade file
void on_connect_clicked(GtkWidget *widget, gpointer data) {
    AppData *app_data = (AppData *)data;
    const char *device = gtk_entry_get_text(GTK_ENTRY(app_data->device_entry));
    const gchar *baud_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app_data->baud_combo));
    int baudrate = atoi(baud_text);

    if (strlen(device) == 0) {
        gtk_label_set_text(GTK_LABEL(app_data->status_label), "No device specified");
        return;
    }

    app_data->fd = init_serial(device, baudrate);
    if (app_data->fd < 0) {
        gtk_label_set_text(GTK_LABEL(app_data->status_label), "Failed to connect");
        return;
    }

    app_data->connected = TRUE;
    gchar *status_text = g_strdup_printf("Connected to %s at %d baud", device, baudrate);
    gtk_label_set_text(GTK_LABEL(app_data->status_label), status_text);
    g_free(status_text);
    
    gtk_widget_set_sensitive(app_data->connect_button, FALSE);
    gtk_widget_set_sensitive(app_data->disconnect_button, TRUE);
    gtk_widget_set_sensitive(app_data->command_entry, TRUE);
    gtk_widget_set_sensitive(app_data->send_button, TRUE);

    GIOChannel *channel = g_io_channel_unix_new(app_data->fd);
    g_io_channel_set_encoding(channel, NULL, NULL);
    g_io_channel_set_flags(channel, G_IO_FLAG_NONBLOCK, NULL);
    app_data->read_source_id = g_io_add_watch(channel, G_IO_IN | G_IO_HUP, serial_read_callback, app_data);
    g_io_channel_unref(channel);
    
    append_to_response(app_data, "Connected successfully");
}

void on_disconnect_clicked(GtkWidget *widget, gpointer data) {
    AppData *app_data = (AppData *)data;

    if (app_data->connected) {
        if (app_data->read_source_id) {
            g_source_remove(app_data->read_source_id);
            app_data->read_source_id = 0;
        }
        close(app_data->fd);
        app_data->connected = FALSE;
        gtk_label_set_text(GTK_LABEL(app_data->status_label), "Disconnected");
        gtk_widget_set_sensitive(app_data->connect_button, TRUE);
        gtk_widget_set_sensitive(app_data->disconnect_button, FALSE);
        gtk_widget_set_sensitive(app_data->command_entry, FALSE);
        gtk_widget_set_sensitive(app_data->send_button, FALSE);
        append_to_response(app_data, "Disconnected");
    }
}

void on_send_command(GtkWidget *widget, gpointer data) {
    AppData *app_data = (AppData *)data;
    const char *command = gtk_entry_get_text(GTK_ENTRY(app_data->command_entry));

    if (!app_data->connected) {
        gtk_label_set_text(GTK_LABEL(app_data->status_label), "Not connected");
        return;
    }

    if (strlen(command) == 0) {
        return;
    }

    // Log the command being sent
    gchar *sent_msg = g_strdup_printf("SENT: %s", command);
    append_to_response(app_data, sent_msg);
    g_free(sent_msg);

    // Send command
    size_t len = strlen(command);
    if (write(app_data->fd, command, len) != (ssize_t)len) {
        perror("write");
        gtk_label_set_text(GTK_LABEL(app_data->status_label), "Write error");
        return;
    }

    // Add semicolon if not present (for CAT commands)
    if (command[len - 1] != ';') {
        write(app_data->fd, ";", 1);
    }
    
    // Add newline
    write(app_data->fd, "\n", 1);
    
    // Clear command entry
    gtk_entry_set_text(GTK_ENTRY(app_data->command_entry), "");
    
    // Update status
    gtk_label_set_text(GTK_LABEL(app_data->status_label), "Command sent");
}

static void on_command_activate(GtkEntry *entry, gpointer data) {
    on_send_command(NULL, data);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    AppData app_data = {0};
    app_data.builder = gtk_builder_new();
    if (!gtk_builder_add_from_file(app_data.builder, "serial_terminal.glade", NULL)) {
        fprintf(stderr, "Failed to load UI file\n");
        return 1;
    }

    app_data.main_window = GTK_WIDGET(gtk_builder_get_object(app_data.builder, "main_window"));
    app_data.device_entry = GTK_WIDGET(gtk_builder_get_object(app_data.builder, "device_entry"));
    app_data.baud_combo = GTK_WIDGET(gtk_builder_get_object(app_data.builder, "baud_combo"));
    app_data.connect_button = GTK_WIDGET(gtk_builder_get_object(app_data.builder, "connect_button"));
    app_data.disconnect_button = GTK_WIDGET(gtk_builder_get_object(app_data.builder, "disconnect_button"));
    app_data.status_label = GTK_WIDGET(gtk_builder_get_object(app_data.builder, "status_label"));
    app_data.command_entry = GTK_WIDGET(gtk_builder_get_object(app_data.builder, "command_entry"));
    app_data.send_button = GTK_WIDGET(gtk_builder_get_object(app_data.builder, "send_button"));
    app_data.response_textview = GTK_WIDGET(gtk_builder_get_object(app_data.builder, "response_textview"));
    app_data.response_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app_data.response_textview));

    gtk_window_set_title(GTK_WINDOW(app_data.main_window), "Serial Terminal");
    gtk_window_set_default_size(GTK_WINDOW(app_data.main_window), 600, 400);
    gtk_window_set_position(GTK_WINDOW(app_data.main_window), GTK_WIN_POS_CENTER);
    g_signal_connect(app_data.main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Connect signals
    g_signal_connect(app_data.connect_button, "clicked", G_CALLBACK(on_connect_clicked), &app_data);
    g_signal_connect(app_data.disconnect_button, "clicked", G_CALLBACK(on_disconnect_clicked), &app_data);
    g_signal_connect(app_data.send_button, "clicked", G_CALLBACK(on_send_command), &app_data);
    g_signal_connect(app_data.command_entry, "activate", G_CALLBACK(on_command_activate), &app_data);

    gtk_widget_show_all(app_data.main_window);

    gtk_main();

    return 0;
}
