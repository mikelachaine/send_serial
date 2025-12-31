
#!/bin/bash

# Create a resource XML file
cat > serial-terminal.gresource.xml << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<gresources>
  <gresource prefix="/com/example/serial-terminal">
    <file>serial-terminal.glade</file>
  </gresource>
</gresources>
EOF

# Compile the resource
glib-compile-resources serial-terminal.gresource.xml --target=serial-terminal-resources.c --generate-source
glib-compile-resources serial-terminal.gresource.xml --target=serial-terminal-resources.h --generate-header

echo "Resources compiled successfully!"
