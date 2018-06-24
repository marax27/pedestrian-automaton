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

//************************************************************

int main(){
	srand(time(NULL));

	sim::Snapshot shot;
	shot.readFromFile("inputs/bigroom-walls.map");

	sim::Config conf;
	conf.readFromFile("inputs/winner.conf");

	sim::Simulation simul(shot, conf);

	cout << "Size: " << shot.dimension << 'x' << shot.dimension << '\n'
	     << shot.exits.size() << " exits.\n"
		 << shot.pedestrians.size() << " pedestrians.\n"
		 << shot.walls.size() << " walls.\n";

	sim::Simulation::Viewer viewer(simul);
	sim::SnapshotDrawer sd(
		sim::PEDESTRIANS|sim::WALLS|sim::EXITS|sim::DYNAMIC_FIELD);
	sim::PopulationChart pchart(simul, {0xff, 0, 0});

	for(int i=0; i!=200; ++i){
		std::stringstream ss;
		ss << "dump/" << i+1 << ".bmp";
		
		sd.draw(viewer.getSnapshot(), ss.str());
		pchart.update();
		simul.runStep();
	}

	pchart.saveToFile("dump/population.bmp");
}
