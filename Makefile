# GNU Make build for cpp-logger.
# Writes target/exe/debug/cpp-logger from src/main/cpp.
# ./build.sh remains the primary build. This file does not add a make verb there.

CXX = g++
PKG_CFLAGS := $(shell pkg-config --cflags glib-2.0)
PKG_LIBS := $(shell pkg-config --libs glib-2.0)
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2 $(PKG_CFLAGS)
BUILD_DIR = target/exe/debug
BINARY = $(BUILD_DIR)/cpp-logger
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/logger.o

.PHONY: all clean

all: $(BINARY)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/main.o: src/main/cpp/main.cpp src/main/cpp/logger.h src/main/cpp/project.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c src/main/cpp/main.cpp -o $@

$(BUILD_DIR)/logger.o: src/main/cpp/logger.cpp src/main/cpp/logger.h src/main/cpp/project.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c src/main/cpp/logger.cpp -o $@

$(BINARY): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(PKG_LIBS) -pthread

clean:
	rm -f $(OBJS) $(BINARY)
