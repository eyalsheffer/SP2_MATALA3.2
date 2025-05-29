CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -pedantic

SFML_CFLAGS = $(shell pkg-config --cflags sfml-graphics)
SFML_LIBS = $(shell pkg-config --libs sfml-graphics)

SRCDIR_PLAYERS = Players
SRCDIR_GUI = Gui
SRC_PLAYERS = $(wildcard $(SRCDIR_PLAYERS)/*.cpp)
SRC_GUI = $(wildcard $(SRCDIR_GUI)/*.cpp)

OBJ_PLAYERS = $(SRC_PLAYERS:.cpp=.o)
OBJ_GUI = $(SRC_GUI:.cpp=.o)
OBJ_COMMON = Game.o
OBJ_MAIN = main.o
OBJ_TEST = Test/test.o

TARGET_MAIN = Main
TARGET_TEST = test

all: $(TARGET_MAIN)

$(TARGET_MAIN): $(OBJ_PLAYERS) $(OBJ_COMMON) $(OBJ_MAIN) $(OBJ_GUI)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(SFML_LIBS)

$(TARGET_TEST): $(OBJ_PLAYERS) $(OBJ_COMMON) $(OBJ_TEST)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Pattern rule for object files in Players and Gui
%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) -c $< -o $@

# For Game, main, and test (no .hpp dependencies assumed)
Game.o: Game.cpp Game.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) -c $< -o $@

test.o: Test/test.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

valgrind: $(TARGET_MAIN)
	valgrind --leak-check=full ./$(TARGET_MAIN)

clean:
	rm -f $(OBJ_PLAYERS) $(OBJ_GUI) $(OBJ_COMMON) $(OBJ_MAIN) $(OBJ_TEST) $(TARGET_MAIN) $(TARGET_TEST)
	find . -name '*.o' -delete
.PHONY: all clean valgrind
