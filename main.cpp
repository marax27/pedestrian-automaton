#include "matrix.h"
#include "field.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <ctime>
#include <cmath>
#include <cstdlib>

#include "snapshot.h"

using sim::fp_t;
using sim::index_t;
using std::cout;

const index_t DIM = 100;

int main(){
	sim::Snapshot shot;
	shot.readFromFile("inputs/rooms.map");

	cout << "Size: " << shot.dimension << 'x' << shot.dimension << '\n'
	     << shot.exits.size() << " exits.\n"
		 << shot.pedestrians.size() << " pedestrians.\n"
		 << shot.walls.size() << " walls.\n";

	// for(const auto &i : shot.walls)
		// cout << '{' << i.x << ", " << i.y << "}\n";
}