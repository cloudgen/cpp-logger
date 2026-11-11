# GNU Make build for cpp-logger.
# Writes target/exe/debug/cpp-logger from src/main/cpp.
# ./build.sh remains the primary build. This file does not add a make verb there.

CXX = g++
PKG_CFLAGS := $(shell pkg-config --cflags glib-2.0)
PKG_LIBS := $(shell pkg-config --libs glib-2.0)
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2 $(PKG_CFLAGS)
BUILD_DIR = target/exe/debug
BINARY = $(BUILD_DIR)/cpp-logger
SRCS = $(wildcard src/main/cpp/*.cpp)
HDRS = $(wildcard src/main/cpp/*.h)
OBJS = $(patsubst src/main/cpp/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean

all: $(BINARY)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/main/cpp/%.cpp $(HDRS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BINARY): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(PKG_LIBS) -pthread

clean:
	rm -f $(OBJS) $(BINARY)
