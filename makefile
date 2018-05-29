CPP = g++
FLAGS = -Wall -Wextra -pedantic -std=c++11
PROJ_NAME = project

all: main.o snapshot.o
	$(CPP) $(FLAGS) main.o snapshot.o -o $(PROJ_NAME)

main.o: main.cpp matrix.h misc.h output.h pedestrian.h field.h
	$(CPP) $(FLAGS) main.cpp -c -o main.o

snapshot.o: snapshot.cpp snapshot.h pedestrian.h field.h
	$(CPP) $(FLAGS) snapshot.cpp -c -o snapshot.o

clean:
	rm -f *.o $(PROJ_NAME) a.out