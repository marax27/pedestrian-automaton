CPP = g++
FLAGS = -Wall -Wextra -pedantic -std=c++17 -O3
PROJ_NAME = project
OBJS = main.o snapshot.o config.o simulation.o bmp.o snapshot_drawer.o happiness.o cellular_sensor.cpp

all: $(OBJS)
	$(CPP) $(FLAGS) $(OBJS) -lpthread -o $(PROJ_NAME)

main.o: main.cpp matrix.h misc.h output.h pedestrian.h progress_bar.h field.h chart.h updateable.h snapshot_drawer.h cellular_sensor.h
	$(CPP) $(FLAGS) main.cpp -c -o main.o

snapshot.o: snapshot.cpp snapshot.h pedestrian.h field.h uid.h file_format.h
	$(CPP) $(FLAGS) snapshot.cpp -c -o snapshot.o

config.o: config.cpp config.h misc.h file_format.h
	$(CPP) $(FLAGS) config.cpp -c -o config.o

simulation.o: simulation.cpp simulation.h snapshot.h matrix.h pedestrian_priority_queue.h
	$(CPP) $(FLAGS) simulation.cpp -c -o simulation.o

bmp.o: bmp.cpp bmp.h
	$(CPP) $(FLAGS) bmp.cpp -c -o bmp.o

snapshot_drawer.o: snapshot_drawer.cpp snapshot_drawer.h simulation.h bmp.h
	$(CPP) $(FLAGS) snapshot_drawer.cpp -c -o snapshot_drawer.o

happiness.o: happiness.cpp happiness.h
	$(CPP) $(FLAGS) happiness.cpp -c -o happiness.o

cellular_sensor.o: cellular_sensor.cpp cellular_sensor.h updateable.h simulation.h
	$(CPP) $(FLAGS) cellular_sensor.cpp -c -o cellular_sensor.o

clean:
	rm -f *.o $(PROJ_NAME) a.out