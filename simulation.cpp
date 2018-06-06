#include "simulation.h"
#include <limits>
#include <cmath>

#include "matrix.h"
#include <ctime>

// Return random element of a matrix. Treat values as weights.
sim::vec2 randomMatrixElement(const Matrix<sim::fp_t, 3, 3> &m){
	sim::fp_t sum = 0.0, t = 0.0;
	for(sim::index_t i = 0; i != 9; ++i)
		sum += m((i - i%3)/3, i%3);

	sim::Output::printWarning("P:\n{} {} {}\n{} {} {}\n{} {} {}",
		m(0,0),m(0,1),m(0,2),m(1,0),m(1,1),m(1,2),m(2,0),m(2,1),m(2,2));

	sim::fp_t random = (double)rand() / RAND_MAX;  // 0 < random < 1
	random *= sum;

	for(sim::index_t i = 0; i != 9; ++i){
		t += m((i - i%3)/3, i%3);
		if(random <= t)
			return sim::vec2(i%3, (i - i%3)/3);
	}
	return sim::vec2(2, 2);
}

namespace sim{

//************************************************************

void Simulation::runStep(){
	// * Move each pedestrian.
	// * Update dynamic field.

	// Not Occuppied By a Pedestrian
	auto NOBP = [&](vec2 pos){
		return (fp_t)1.0 * !data.occupiedByPedestrian(pos);
	};

	const auto &sf = static_field;
	for(auto &p : data.pedestrians){
		index_t x = p.getPosition().x,
		        y = p.getPosition().y;
		Matrix<fp_t, 3, 3> S{
			{0, sf.at(y-1, x), 0},
			{sf.at(y, x-1), sf.at(y  , x), sf.at(y, x+1)},
			{0, sf.at(y+1, x), 0}
		};
		Matrix<fp_t, 3, 3> N{
			{0, NOBP({x, y-1}), 0},
			{NOBP({x-1, y}), 1, NOBP({x+1, y})},
			{0, NOBP({x, y+1}), 0}
		};
		Matrix<fp_t, 3, 3> P = S * N;

		auto r = randomMatrixElement(P);
		Output::printWarning("r: ({}, {}). Cpos: ({}, {})", r.x, r.y, x, y);
		p.move({x + r.x - 1, y + r.y - 1});
		Output::printWarning("Pedestrian set to ({}, {})", x, y);
	}

	++time_elapsed;
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
			static_field(y, x) = exp(-0.2*min_dist);
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