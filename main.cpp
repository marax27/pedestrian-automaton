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

void drawSquare(
	JiMP2::BMP &bitmap, sim::vec2 pos, int SIZE, uint8_t r, uint8_t g, uint8_t b){

	pos.x *= SIZE;
	pos.y *= SIZE;
	for(auto i = pos.x; i < pos.x+SIZE; ++i)
		for(auto j = pos.y; j < pos.y+SIZE; ++j)
			bitmap.setPixel(i, j+1, r, g, b);
}

//************************************************************

int main(){
	srand(time(NULL));

	sim::Snapshot shot;
	shot.readFromFile("inputs/bigroom.map");

	sim::Config conf;
	conf.readFromFile("inputs/winner.conf");

	sim::Simulation simul(std::move(shot), conf);

	cout << "Size: " << shot.dimension << 'x' << shot.dimension << '\n'
	     << shot.exits.size() << " exits.\n"
		 << shot.pedestrians.size() << " pedestrians.\n"
		 << shot.walls.size() << " walls.\n";

	sim::Simulation::Viewer viewer(simul);

	const index_t D = viewer.getDimension();
	const int SIZE = 32;

	for(int i=0; i!=200; ++i){
		JiMP2::BMP b(D*SIZE, D*SIZE);
		std::stringstream ss;
		ss << "dump/" << i+1 << ".bmp";
		std::ofstream writer(ss.str());
		if(!writer)
			break;

		// Walls.
		for(const auto &w : viewer.getWalls())
			drawSquare(b, w, SIZE, 0x66, 0x33, 0);

		// Exits.
		for(const auto &x : viewer.getExits())
			drawSquare(b, x, SIZE, 0x66, 0xff, 0xff);

		// Pedestrians.
		for(const auto &p : viewer.getPedestrians())
			drawSquare(b, p.second.getPosition(), SIZE, 0, 0, 0);

		auto trim20 = [](int c){ return c<0 ? 0 : c; };
		const auto &df = viewer.getDynamicField();
		for(index_t y=0; y!=viewer.getDimension(); ++y)
			for(index_t x=0; x!=viewer.getDimension(); ++x){
				drawSquare(b, {x*2, y*2}, SIZE/2, 255, trim20(255-20*df(x, y)), trim20(255-30*df(x, y)));
			}

		writer << b;
		writer.close();

		simul.runStep();
	}
}