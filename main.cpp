#include "matrix.h"
#include "field.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <ctime>
#include <cmath>
#include <cstdlib>

#include "snapshot.h"
#include "simulation.h"

#include "bmp.h"

using sim::fp_t;
using sim::index_t;
using std::cout;

const index_t DIM = 100;

int main(){
	sim::Snapshot shot;
	shot.readFromFile("inputs/rooms.map");

	sim::Simulation simul(std::move(shot));

	/*cout << "Size: " << shot.dimension << 'x' << shot.dimension << '\n'
	     << shot.exits.size() << " exits.\n"
		 << shot.pedestrians.size() << " pedestrians.\n"
		 << shot.walls.size() << " walls.\n";*/

	sim::Simulation::Viewer viewer(simul);

	const index_t D = viewer.getDimension();
	JiMP2::BMP b(D, D);
	for(index_t y=0; y!=D; ++y){
		for(index_t x=0; x!=D; ++x){
			fp_t s = viewer.getStaticField()(y, x);
			uint8_t g = uint8_t(s*255.0);
			b.setPixel(x, y+1, g, g, g);
		}
	}
	std::ofstream writer("output_static.bmp");
	writer << b;
	writer.close();

	// shot.writeToFile("output_test.map");
}