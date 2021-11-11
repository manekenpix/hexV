CC = clang++ -v
CCDEBUG = -std=c++17 -g
CCRELEASE = -std=c++17 -O3
CCGTKMM = -I/usr/include/gtkmm-3.0 -I/usr/lib/gtkmm-3.0/include -I/usr/include/giomm-2.4 -I/usr/lib/giomm-2.4/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/glibmm-2.4 -I/usr/lib/glibmm-2.4/include -I/usr/include/sigc++-2.0 -I/usr/lib/sigc++-2.0/include -I/usr/include/gtk-3.0 -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/fribidi -I/usr/include/cairo -I/usr/include/lzo -I/usr/include/pixman-1 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/gio-unix-2.0 -I/usr/include/cloudproviders -I/usr/include/atk-1.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/at-spi-2.0 -I/usr/include/cairomm-1.0 -I/usr/lib/cairomm-1.0/include -I/usr/include/pangomm-1.4 -I/usr/lib/pangomm-1.4/include -I/usr/include/atkmm-1.6 -I/usr/lib/atkmm-1.6/include -I/usr/include/gtk-3.0/unix-print -I/usr/include/gdkmm-3.0 -I/usr/lib/gdkmm-3.0/include -pthread -lgtkmm-3.0 -latkmm-1.6 -lgdkmm-3.0 -lgiomm-2.4 -lgtk-3 -lgdk-3 -lz -latk-1.0 -lcairo-gobject -lgio-2.0 -lpangomm-1.4 -lglibmm-2.4 -lcairomm-1.0 -lsigc-2.0 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -lcairo -lgdk_pixbuf-2.0 -lgobject-2.0 -lglib-2.0
SRC_DIR = src
INCLUDE_DIR = $(SRC_DIR)/include
BUILD_DIR = build
TARGET = hexV
SRC_EXT = cpp
HDR_EXT = h

SOURCES = $(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT))
OBJECTS = $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.$(SRC_EXT)=.o))

release: $(OBJECTS)
	$(CC) $(CCRELEASE) $^ -o $(TARGET) $(CCGTKMM)

debug: $(OBJECTS)
	$(CC) $(CCDEBUG) $^ -o $(TARGET) ${CCGTKMM}

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCDEBUG) -Wall -c -o $@ $< $(CCGTKMM)

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILD_DIR) $(TARGET)"; $(RM) -r $(BUILD_DIR) $(TARGET)
