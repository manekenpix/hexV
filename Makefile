CC = clang++ -v -std=c++17 -Wall
RELEASE = -O3
DEBUG = -g
CCGTKMM = `pkg-config gtkmm-3.0 --cflags --libs`
SRC_DIR = src
INCLUDE_DIR = $(SRC_DIR)/include
BUILD_DIR = build
TARGET = hexV
SRC_EXT = cpp
HDR_EXT = h

SOURCES = $(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT))
OBJECTS = $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.$(SRC_EXT)=.o))

release: export CMODE := -O3
release: $(OBJECTS)
	$(CC) $(RELEASE) $^ -o $(TARGET) $(CCGTKMM)

debug: export CMODE := -g
debug: $(OBJECTS)
	$(CC) $(DEBUG) $^ -o $(TARGET) ${CCGTKMM}

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
	@mkdir -p $(dir $@)
	$(CC) $(CMODE) -Wall -c -o $@ $< $(CCGTKMM)

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILD_DIR) $(TARGET)"; $(RM) -r $(BUILD_DIR) $(TARGET)
