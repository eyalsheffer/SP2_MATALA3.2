CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -pedantic
SFML_CFLAGS = `pkg-config --cflags sfml-graphics`
SFML_LIBS = `pkg-config --libs sfml-graphics`
SRCDIR_PLAYERS = Players
SRCDIR_GUI = Gui
OBJ_PLAYERS = Players/Baron.o Players/General.o Players/Governor.o Players/Judge.o Players/Merchant.o Players/Player.o Players/Spy.o
OBJ_GUI = Gui/MainGui.o Gui/GameGui.o
OBJ = $(OBJ_PLAYERS) Game.o main.o test.o $(OBJ_GUI)

Main: $(OBJ_PLAYERS) Game.o main.o $(OBJ_GUI)
	$(CXX) $(CXXFLAGS) -o Main $(OBJ_PLAYERS) Game.o main.o $(OBJ_GUI) $(SFML_LIBS)

test: $(OBJ_PLAYERS) Game.o test.o
	$(CXX) $(CXXFLAGS) -o test $(OBJ_PLAYERS) Game.o test.o $(SFML_LIBS)

valgrind: test
	valgrind --leak-check=full ./test

Players/Baron.o: Players/Baron.cpp Players/Baron.hpp
	$(CXX) $(CXXFLAGS) -c Players/Baron.cpp -o Players/Baron.o

Players/General.o: Players/General.cpp Players/General.hpp
	$(CXX) $(CXXFLAGS) -c Players/General.cpp -o Players/General.o

Players/Governor.o: Players/Governor.cpp Players/Governor.hpp
	$(CXX) $(CXXFLAGS) -c Players/Governor.cpp -o Players/Governor.o

Players/Judge.o: Players/Judge.cpp Players/Judge.hpp
	$(CXX) $(CXXFLAGS) -c Players/Judge.cpp -o Players/Judge.o

Players/Merchant.o: Players/Merchant.cpp Players/Merchant.hpp
	$(CXX) $(CXXFLAGS) -c Players/Merchant.cpp -o Players/Merchant.o

Players/Player.o: Players/Player.cpp Players/Player.hpp
	$(CXX) $(CXXFLAGS) -c Players/Player.cpp -o Players/Player.o

Players/Spy.o: Players/Spy.cpp Players/Spy.hpp
	$(CXX) $(CXXFLAGS) -c Players/Spy.cpp -o Players/Spy.o

Game.o: Game.cpp Game.hpp
	$(CXX) $(CXXFLAGS) -c Game.cpp -o Game.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) -c main.cpp -o main.o

test.o: test.cpp
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) -c test.cpp -o test.o

Gui/MainGui.o: Gui/MainGui.cpp Gui/MainGui.hpp
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) -c Gui/MainGui.cpp -o Gui/MainGui.o

Gui/GameGui.o: Gui/GameGui.cpp Gui/GameGui.hpp
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) -c Gui/GameGui.cpp -o Gui/GameGui.o

clean:
	rm -f Players/*.o Gui/*.o *.o Main test