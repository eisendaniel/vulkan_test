DBFLAGS = -g -O0 -DDEBUG
RLFLAGS = -O3 -DNDEBUG

SRC_DIR := src
BIN_DIR := bin
IDIR := include

CXXFLAGS = -std=c++17 -Wall -I$(IDIR)
LDFLAGS = -lglfw -lvulkan -lXxf86vm -lX11 -lpthread -lXrandr -lXi -ldl

EXE := $(BIN_DIR)/vulkan_test
SRC := $(wildcard $(SRC_DIR)/*)

.PHONY: all clean run debug release remake

all: debug

debug: $(SRC)
	$(CXX) $(CXXFLAGS) $(DBFLAGS) -o $(EXE) $(SRC) $(LDFLAGS)

release: $(SRC)
	$(CXX) $(CXXFLAGS) $(RLFLAGS) -o $(EXE) $(SRC) $(LDFLAGS)

remake: clean all

run: $(EXE)
	./$(EXE)
clean:
	$(RM) $(BIN_DIR)/*