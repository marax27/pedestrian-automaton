#include "simulation.h"
#include <limits>
#include <cmath>

namespace sim{

//************************************************************

void Simulation::runStep(){
	++time_elapsed;
	// ...
}

//************************************************************

void Simulation::initializeStaticField(){
	// First approach.
	// Exits: 1.0
	// Free space: 1/e^(distance from nearest exit)
	// Walls: 0.0

	const fp_t EXIT = 1.0f,
	           WALL = 0.0f,
			   INF = std::numeric_limits<fp_t>::infinity();

	const index_t DIM = data.dimension;
	static_field = Field<fp_t>(DIM);

	// Compute distance from nearest exit point for each cell on the grid.
	for(index_t y = 0; y != DIM; ++y){
		for(index_t x = 0; x != DIM; ++x){
			fp_t min_dist = INF;
			for(const auto &ex : data.exits){
				fp_t d = sqrt(squared(ex.x - x) + squared(ex.y - y));
				if(d < min_dist)
					min_dist = d;
			}

			static_field(y, x) = exp(-min_dist);
		}
	}

	// Assign appropriate special values to walls and exits.
	for(const auto &ex : data.exits)
		static_field(ex.y, ex.x) = EXIT;
	for(const auto &w : data.walls)
		static_field(w.y, w.x) = WALL;
}

//************************************************************

}