#include "matrix.h"
#include "field.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <ctime>
#include <cmath>
#include <cstdlib>

#include "chart.h"
#include "config.h"
#include "snapshot.h"
#include "simulation.h"
#include "snapshot_drawer.h"

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
	for(uint16_t i = pos.x; i < pos.x+SIZE; ++i)
		for(uint16_t j = pos.y; j < pos.y+SIZE; ++j)
			bitmap.setPixel({i, j}, r, g, b);
}

//************************************************************

int main(){
	srand(time(NULL));

	sim::Snapshot shot;
	shot.readFromFile("inputs/bigroom.map");

	sim::Config conf;
	conf.readFromFile("inputs/winner.conf");

	sim::Simulation simul(shot, conf);

	cout << "Size: " << shot.dimension << 'x' << shot.dimension << '\n'
	     << shot.exits.size() << " exits.\n"
		 << shot.pedestrians.size() << " pedestrians.\n"
		 << shot.walls.size() << " walls.\n";

	// sim::Simulation::Viewer viewer(simul);
	sim::SnapshotDrawer sd(simul,
		sim::PEDESTRIANS|sim::WALLS|sim::EXITS|sim::DYNAMIC_FIELD);
	sim::PopulationChart pchart(simul);

	for(int i=0; i!=100; ++i){
		std::stringstream ss;
		ss << "dump/" << i+1 << ".bmp";
		
		sd.readAndDraw(ss.str());

		pchart.read();

		simul.runStep();
	}

	pchart.draw("dump/population.bmp");
}