The a11y bus is accessed via two mechanisms:

1) The DBus session bus, service "org.a11y.Bus", method "GetAddress")
2) The X11 root window property AT_SPI_BUS

If the "toolkit-accessibility" variable is set, the bus is launched
immediately (and will be accessible immediately via the X11 property).
Otherwise, it will be spawned dynamically.


