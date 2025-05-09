portalsnake.x: main.o worm.o screen.o cell.o game.o
	g++ -std=c++20 main.o worm.o screen.o cell.o game.o -o portalsnake.x -lcurses -ltermcap
main.o: main.cpp Cell.hpp Worm.hpp Screen.hpp Game.hpp
	g++ -std=c++20 main.cpp -o main.o -c
game.o: Game.cpp Game.hpp Worm.hpp Screen.hpp Cell.hpp
	g++ -std=c++20 Game.cpp -o game.o -c
worm.o: Worm.cpp Worm.hpp Cell.hpp
	g++ -std=c++20 Worm.cpp -o worm.o -c
screen.o: Screen.cpp Screen.hpp Cell.hpp
	g++ -std=c++20 Screen.cpp -o screen.o -c
cell.o: Cell.cpp Cell.hpp
	g++ -std=c++20 Cell.cpp -o cell.o -c
clean:
	rm portalsnake.x *.o
