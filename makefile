CPP = g++
FLAGS = -Wall -Wextra -pedantic -std=c++11 -g
PROJ_NAME = project
OBJS = main.o snapshot.o simulation.o bmp.o

all: $(OBJS) matrix.h misc.h output.h pedestrian.h field.h pedestrian_priority_queue.h uid.h
	$(CPP) $(FLAGS) $(OBJS) -o $(PROJ_NAME)

main.o: main.cpp matrix.h misc.h output.h pedestrian.h field.h
	$(CPP) $(FLAGS) main.cpp -c -o main.o

snapshot.o: snapshot.cpp snapshot.h pedestrian.h field.h uid.h
	$(CPP) $(FLAGS) snapshot.cpp -c -o snapshot.o

simulation.o: simulation.cpp simulation.h snapshot.h matrix.h pedestrian_priority_queue.h
	$(CPP) $(FLAGS) simulation.cpp -c -o simulation.o

bmp.o: bmp.cpp bmp.h
	$(CPP) $(FLAGS) bmp.cpp -c -o bmp.o

clean:
	rm -f *.o $(PROJ_NAME) a.out