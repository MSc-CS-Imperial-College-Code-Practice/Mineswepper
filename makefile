exec: main.o minesweeper.o
	  	g++ -Wall -g main.o minesweeper.o -o exec

main.o: main.cpp minesweeper.h
		g++ -Wall -g -c main.cpp

minesweeper.o: minesweeper.cpp minesweeper.h
		  g++ -Wall -g -c minesweeper.cpp 

clean:
	  rm -f *.o exec