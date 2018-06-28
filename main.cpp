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
#include "cellular_sensor.h"
#include "snapshot_drawer.h"

#include "bmp.h"

using sim::fp_t;
using sim::index_t;
using std::cout;

sim::vec2 randomMatrixElement(const Matrix<sim::fp_t, 3, 3> &m);
void processArgc(int argc, char **argv);

namespace{
	// const index_t DIM = 100;
	index_t STEPS = 256;

	struct{
		bool save_bitmaps = false,
		     signal_progress = true;
	} ConsoleSettings;
}

//************************************************************

int main(int argc, char **argv){
	srand(time(NULL));

	// Load files.
	sim::Snapshot shot;
	shot.readFromFile("inputs/r40.map");

	sim::Config conf;
	conf.readFromFile("inputs/iter4.conf");

	processArgc(argc, argv);

	// Initialize simulation.
	sim::Simulation simul(shot, conf);

	cout << "Size: " << shot.dimension << 'x' << shot.dimension << '\n'
	     << shot.exits.size() << " exits.\n"
		 << shot.pedestrians.size() << " pedestrians.\n"
		 << shot.walls.size() << " walls.\n";

	sim::Simulation::Viewer viewer(simul);
	sim::SnapshotDrawer sd(
		sim::PEDESTRIANS|sim::WALLS|sim::EXITS|sim::DYNAMIC_FIELD|sim::HAPPINESS, 8);
	sim::PopulationChart pchart(simul, {0xff, 0, 0});

	sim::MotionSensor ms({20, 5}, simul);

	ProgressBar<int> pbar(STEPS, 0, 40);

	for(index_t i = 0; i != STEPS; ++i){
		std::stringstream ss;
		ss << "dump/" << i+1 << ".bmp";
		
		if(ConsoleSettings.save_bitmaps)
			sd.draw(viewer.getSnapshot(), ss.str());
		pchart.update();
		ms.update();
		simul.runStep();

		if(ConsoleSettings.signal_progress){
			pbar.shift(1);
			std::cout << pbar << '\r';
			std::cout.flush();
		}
	}

	ms.saveToFile("dump/cell_20x5.txt");
	pchart.saveToFile("dump/population.bmp");
	std::cout << '\n';
}

//************************************************************

void processArgc(int argc, char **argv){
	for(int i = 1; i != argc; ++i){
		if(argv[i] == std::string{"--no-save-bitmap"})
			ConsoleSettings.save_bitmaps = false;
		else if(argv[i] == std::string{"--save-bitmap"})
			ConsoleSettings.save_bitmaps = true;
		else if(argv[i] == std::string("--no-signal-progress"))
			ConsoleSettings.signal_progress = false;
		else if(argv[i] == std::string{"--signal-progress"})
			ConsoleSettings.signal_progress = true;
		else if(argv[i] == std::string{"--steps"}){
			// Read next argument as number of steps.
			if(i == argc-1)
				sim::Output::printWarning("Number of steps not given. Default: {}.", STEPS);
			else{
				try{
					++i;
					STEPS = stringToValue<index_t>(argv[i]);
				}catch(std::invalid_argument&){
					sim::Output::printWarning("Invalid number of steps. Default: {}.", STEPS);
				}
			}
		}
		else
			sim::Output::printWarning("Unknown argument: '{}'", argv[i]);
	}
}

//************************************************************
