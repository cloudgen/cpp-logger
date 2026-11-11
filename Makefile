# GNU Make build for cpp-logger.
# The debug binary and the download copy are requirement-cpp-debug-binary.md.
# ./build.sh remains the primary build. This file does not add a make verb there.

CXX = g++
PKG_CFLAGS := $(shell pkg-config --cflags glib-2.0)
PKG_LIBS := $(shell pkg-config --libs glib-2.0)
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2 $(PKG_CFLAGS)
BUILD_DIR = target/exe/debug
PRJ := $(shell awk -F= '/^PRJ=/{print $$2; exit}' project.ini)
ifeq ($(PRJ),)
PRJ := cpp-logger
endif
ifeq ($(OS),Windows_NT)
EXE_SUFFIX := .exe
else
EXE_SUFFIX :=
endif
BINARY = $(BUILD_DIR)/$(PRJ)$(EXE_SUFFIX)
DOWNLOAD_DIR = download
DOWNLOAD_BINARY = $(DOWNLOAD_DIR)/$(PRJ)$(EXE_SUFFIX)
SRCS = $(wildcard src/main/cpp/*.cpp)
HDRS = $(wildcard src/main/cpp/*.h)
OBJS = $(patsubst src/main/cpp/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean

all: $(DOWNLOAD_BINARY)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/main/cpp/%.cpp $(HDRS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BINARY): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(PKG_LIBS) -pthread

$(DOWNLOAD_BINARY): $(BINARY)
	mkdir -p $(DOWNLOAD_DIR)
	cp $(BINARY) $(DOWNLOAD_BINARY)

clean:
	rm -f $(OBJS) $(BINARY) $(DOWNLOAD_BINARY)
