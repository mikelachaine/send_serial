
# Serial Terminal UI - CAT Control Application

A GTK3-based graphical serial terminal for controlling Yaesu FTX-1 transceivers via CAT (Computer Aided Transceiver) protocol over USB or UART connections.

## Features

- **Graphical Interface**: User-friendly GTK3 interface for serial communication
- **Multi-Baud Rate Support**: 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600 bps
- **CAT Protocol Support**: Support for CAT commands, no need to type the ';' at the end of command
- **Command Logging**: Timestamped send/receive message display
- **Adaptive Theme**: Automatically detects and applies system dark/light theme
- **Non-blocking I/O**: Responsive UI with asynchronous serial communication
- **Embedded Resources**: Self-contained executable with embedded UI definition

## Requirements

### Build Dependencies
- GTK+ 3.0 or later
- GLib 2.0 or later
- pkg-config
- GCC compiler
- glib-compile-resources (part of GLib)

### Runtime Dependencies
- GTK+ 3.0 runtime libraries
- GLib 2.0 runtime libraries

### System Requirements
- Linux/Unix-based operating system
- USB or UART serial port access
- Appropriate permissions to access `/dev/ttyUSB*` or `/dev/ttyS*` devices

## Installation

### Build from Source

```bash
cd /path/to/send_serial
make clean
make
```