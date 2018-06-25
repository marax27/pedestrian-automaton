#include "matrix.h"
#include "field.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <ctime>
#include <cmath>
#include <cstdlib>

#include "chart.h"
#include "config.h"
#include "snapshot.h"
#include "simulation.h"
#include "progress_bar.h"
#include "snapshot_drawer.h"

#include "bmp.h"

using sim::fp_t;
using sim::index_t;
using std::cout;

const index_t DIM = 100;

sim::vec2 randomMatrixElement(const Matrix<sim::fp_t, 3, 3> &m);

//************************************************************

int main(int argc, char **argv){
	srand(time(NULL));

	// Load files.
	sim::Snapshot shot;
	shot.readFromFile("inputs/bigroom-walls.map");

	sim::Config conf;
	conf.readFromFile("inputs/winner.conf");

	// Initialize simulation.
	sim::Simulation simul(shot, conf);

	cout << "Size: " << shot.dimension << 'x' << shot.dimension << '\n'
	     << shot.exits.size() << " exits.\n"
		 << shot.pedestrians.size() << " pedestrians.\n"
		 << shot.walls.size() << " walls.\n";

	bool b_save_bitmaps = true,
	     b_signal_progress = false;

	for(int i = 1; i != argc; ++i){
		if(argv[i] == std::string{"--no-bitmap"})
			b_save_bitmaps = false;
		else if(argv[i] == std::string("--signal-progress"))
			b_signal_progress = true;
	}

	sim::Simulation::Viewer viewer(simul);
	sim::SnapshotDrawer sd(
		sim::PEDESTRIANS|sim::WALLS|sim::EXITS|sim::DYNAMIC_FIELD);
	sim::PopulationChart pchart(simul, {0xff, 0, 0});

	const int STEPS = 256;
	ProgressBar<int> pbar(STEPS, 0, 40);

	for(int i=0; i!=STEPS; ++i){
		std::stringstream ss;
		ss << "dump/" << i+1 << ".bmp";
		
		if(b_save_bitmaps)
			sd.draw(viewer.getSnapshot(), ss.str());
		pchart.update();
		simul.runStep();

		if(b_signal_progress){
			pbar.shift(1);
			std::cerr << pbar << '\r';
		}
	}

	pchart.saveToFile("dump/population.bmp");
}
