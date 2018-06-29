#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <ctime>
#include <cmath>
#include <thread>
#include <cstdlib>

#include "matrix.h"
#include "field.h"

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

void thread_saveBitmaps(
	const std::vector<sim::Snapshot> &v, int p, int q);

namespace{
	index_t STEPS = 256;

	const int PROGRESS_BAR_LENGTH = 40;

	std::string SNAPSHOT_FILENAME = "inputs/evac_room.map";
	std::string CONFIG_FILENAME = "inputs/standard_settings.conf";

	struct{
		bool save_bitmaps = false,
		     signal_progress = true;
	} ConsoleSettings;
}

//************************************************************

int main(int argc, char **argv){
	srand(time(NULL));

	processArgc(argc, argv);

	// Load files.
	sim::Snapshot shot;
	shot.readFromFile(SNAPSHOT_FILENAME);

	sim::Config conf;
	conf.readFromFile(CONFIG_FILENAME);

	// Initialize simulation.
	sim::Simulation simul(shot, conf);

	// Print some information.
	cout << "Size: " << shot.dimension << 'x' << shot.dimension << '\n'
	     << shot.exits.size() << " exits.\n"
		 << shot.pedestrians.size() << " pedestrians.\n"
		 << shot.walls.size() << " walls.\n"
		 << "Map: " << SNAPSHOT_FILENAME << '\n'
		 << "Config: " << CONFIG_FILENAME << '\n';

	// Initialize viewer objects.
	sim::Simulation::Viewer viewer(simul);
	sim::PopulationChart pchart(simul, {0xff, 0, 0});
	sim::MotionSensor ms(simul, {19, 2});  //close to the exit
	sim::DynamicSensor ds(simul, {19, 2}, {0x22, 0x22, 0x99});

	ProgressBar<int> pbar(STEPS, 0, PROGRESS_BAR_LENGTH);

	std::vector<sim::Snapshot> snaps;
	snaps.reserve(STEPS);

	// Simulation loop.
	for(index_t i = 0; i != STEPS; ++i){
		if(ConsoleSettings.save_bitmaps)
			snaps.push_back(viewer.getSnapshot());

		pchart.update();
		ms.update();
		ds.update();

		simul.runStep();

		if(ConsoleSettings.signal_progress){
			pbar.shift(1);
			std::cout << pbar << '\r';
			std::cout.flush();
		}
	}

	std::cout << '\n';

	// Final operations.
	ms.saveToFile("dump/motion-cell_19x2.txt");
	pchart.saveToFile("dump/population.bmp");
	ds.saveToFile("dump/dynamic-cell_19x2.bmp");

	// Save all collected bitmaps. Use multithreading to speed up the process.
	if(ConsoleSettings.save_bitmaps){
		// Save the last snapshot as .map file.
		snaps.back().writeToFile("dump/last.map");

		const int MAX_THREADS = std::thread::hardware_concurrency();
		std::vector<std::thread> threads;
		ProgressBar<int> pbar_bmp(MAX_THREADS, 0, PROGRESS_BAR_LENGTH);

		// Construct threads.
		for(int i = 0; i != MAX_THREADS; ++i){
			int p = i * snaps.size() / MAX_THREADS;
			int q = std::min((i+1)*snaps.size()/MAX_THREADS, snaps.size());

			threads.push_back(std::thread(thread_saveBitmaps, snaps, p, q));
		}
		
		std::cout << pbar_bmp << '\r';
		std::cout.flush();

		// Join threads.
		for(auto &t : threads){
			t.join();
			pbar_bmp.shift(1);
			std::cout << pbar_bmp << '\r';
			std::cout.flush();
		}
	}

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
				sim::Output::printWarning("Number of steps not provided. Default: {}.", STEPS);
			else{
				try{
					++i;
					STEPS = stringToValue<index_t>(argv[i]);
				}catch(std::invalid_argument&){
					sim::Output::printWarning("Invalid number of steps. Default: {}.", STEPS);
				}
			}
		}
		else if(argv[i] == std::string{"--map"}){
			if(i == argc-1)
				sim::Output::printWarning("Map filename not provided. Default: {}.", SNAPSHOT_FILENAME);
			else
				SNAPSHOT_FILENAME = argv[++i];
		}
		else if(argv[i] == std::string{"--config"}){
			if(i == argc-1)
				sim::Output::printWarning("Simulation config not provided. Default: {}.", CONFIG_FILENAME);
			else
				CONFIG_FILENAME = argv[++i];
		}
		else
			sim::Output::printWarning("Unknown argument: '{}'", argv[i]);
	}
}

//************************************************************

void thread_saveBitmaps(
	const std::vector<sim::Snapshot> &v, int p, int q){

	if(p > q)
		return;
	
	std::stringstream ss;
	sim::SnapshotDrawer sd(
		sim::PEDESTRIANS|sim::WALLS|sim::EXITS|sim::DYNAMIC_FIELD|sim::HAPPINESS, 8);

	for(int i = p; i != q; ++i){
		ss.str("");
		ss << "dump/" << i+1 << ".bmp";
		sd.draw(v[i], ss.str());
	}
}

//************************************************************
