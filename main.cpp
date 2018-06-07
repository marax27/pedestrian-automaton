#include "matrix.h"
#include "field.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <ctime>
#include <cmath>
#include <cstdlib>

#include "config.h"
#include "snapshot.h"
#include "simulation.h"

#include "bmp.h"

using sim::fp_t;
using sim::index_t;
using std::cout;

const index_t DIM = 100;

sim::vec2 randomMatrixElement(const Matrix<sim::fp_t, 3, 3> &m);

void drawSquare(JiMP2::BMP &b, sim::vec2 pos){
	const int C = 15;
	pos.x *= C;
	pos.y *= C;
	for(auto i = pos.x; i < pos.x+C; ++i)
		for(auto j = pos.y; j < pos.y+C; ++j)
			b.setPixel(i, j+1, 0, 0, 0);
}

int main(){
	srand(time(NULL));

	sim::Snapshot shot;
	shot.readFromFile("inputs/rooms.map");

	sim::Config conf;
	conf.readFromFile("inputs/standard_settings.conf");

	sim::Simulation simul(std::move(shot), conf);

	cout << "Size: " << shot.dimension << 'x' << shot.dimension << '\n'
	     << shot.exits.size() << " exits.\n"
		 << shot.pedestrians.size() << " pedestrians.\n"
		 << shot.walls.size() << " walls.\n";

	sim::Simulation::Viewer viewer(simul);

	const index_t D = viewer.getDimension();

	// Static field printout.
	/*JiMP2::BMP sf(D, D);
	for(index_t y=0; y!=D; ++y){
		for(index_t x=0; x!=D; ++x){
			fp_t s = viewer.getStaticField()(x, y);
			uint8_t g = uint8_t(s*255.0);
			sf.setPixel(x, y+1, g, g, g);
		}
	}
	std::ofstream sfw("dump/static.bmp");
	sfw << sf;
	sfw.close();*/

	for(int i=0; i!=100; ++i){
		JiMP2::BMP b(D*15, D*15);
		std::stringstream ss;
		ss << "dump/" << i+1 << ".bmp";
		std::ofstream writer(ss.str());
		if(!writer)
			break;
		
		for(const auto &p : viewer.getPedestrians())
			drawSquare(b, {p.second.getPosition().x, p.second.getPosition().y});

		writer << b;
		writer.close();

		simul.runStep();
	}

	// shot.writeToFile("output_test.map");
}