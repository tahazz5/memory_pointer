CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2

BIN_DIR := bin

SHARED_TEST := $(BIN_DIR)/test_shared
UNIQUE_TEST := $(BIN_DIR)/test_unique
MYSTACK_TEST := $(BIN_DIR)/test_mystack
OPTIONAL_TEST := $(BIN_DIR)/test_optional
VARIANT_TEST := $(BIN_DIR)/test_variant

.PHONY: all test clean

all: test

test: $(SHARED_TEST) $(UNIQUE_TEST) $(MYSTACK_TEST) $(OPTIONAL_TEST) $(VARIANT_TEST)
	./$(SHARED_TEST)
	./$(UNIQUE_TEST)
	./$(MYSTACK_TEST)
	./$(OPTIONAL_TEST)
	./$(VARIANT_TEST)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(SHARED_TEST): shared_pointer/test_shared.cpp shared_pointer/shared_pointer.hpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) shared_pointer/test_shared.cpp -o $(SHARED_TEST)

$(UNIQUE_TEST): unique_pointer/test_uniquepointer.cpp unique_pointer/uniquepointer.hpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) unique_pointer/test_uniquepointer.cpp -o $(UNIQUE_TEST)

$(MYSTACK_TEST): mystack/test_mystack.cpp mystack/mystack.hpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) mystack/test_mystack.cpp -o $(MYSTACK_TEST)

$(OPTIONAL_TEST): myoptional/test_myoptional.cpp myoptional/myoptional.hpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) myoptional/test_myoptional.cpp -o $(OPTIONAL_TEST)

$(VARIANT_TEST): myvariant/test_myvariant.cpp myvariant/myvariant.hpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) myvariant/test_myvariant.cpp -o $(VARIANT_TEST)

clean:
	rm -rf $(BIN_DIR)
