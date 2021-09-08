SDC := glslc

DBFLAGS = -g -O0 -DDEBUG
RLFLAGS = -O3 -DNDEBUG

SRC_DIR := src
BIN_DIR := bin
IDIR := include
SHD_DIR := shaders

CXXFLAGS = -std=c++17 -Wall -I$(IDIR)
LDFLAGS = -lglfw -lvulkan -lXxf86vm -lX11 -lpthread -lXrandr -lXi -ldl

EXE := $(BIN_DIR)/vulkan_test
SRC := $(wildcard $(SRC_DIR)/*)
SHD := $(wildcard $(SHD_DIR)/*)

.PHONY: all clean run debug release remake shader

all: shader debug

remake: clean all

debug: $(SRC)
	$(CXX) $(CXXFLAGS) $(DBFLAGS) -o $(EXE) $(SRC) $(LDFLAGS)

release: $(SRC)
	$(CXX) $(CXXFLAGS) $(RLFLAGS) -o $(EXE) $(SRC) $(LDFLAGS)

shader: $(SHD)
	$(SDC) $(SHD_DIR)/shader.vert -o $(SHD_DIR)/bin/vert.spv
	$(SDC) $(SHD_DIR)/shader.frag -o $(SHD_DIR)/bin/frag.spv

clean:
	$(RM) $(BIN_DIR)/* $(SHD_DIR)/bin/*

run: $(EXE)
	./$(EXE)