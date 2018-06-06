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

	// sim::Output::printWarning("P:\n{} {} {}\n{} {} {}\n{} {} {}",
		// m(0,0),m(0,1),m(0,2),m(1,0),m(1,1),m(1,2),m(2,0),m(2,1),m(2,2));

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

	// For each pedestrian:
	// get S_ij, N_ij (is neighbourhood occupied)
	// compute P_ij: matrix of probabilities
	// pick random field of P_ij
	// ++time_elapsed

	auto *new_queue = new PedestrianPriorityQueue<fp_t>(static_field, data.pedestrians);

	while(!pqueue->empty()){
		uid_t id = pqueue->top();
		vec2 pos = data.pedestrians.at(id).getPosition();
		new_queue->push(id);

		// Get values of static field in the neighbourhood of a pedestrian.
		Matrix<fp_t, 3, 3> S = neighbourhoodStaticField(pos);  //std::move maybe?
		pqueue->pop();
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

Matrix<fp_t, 3, 3> Simulation::neighbourhoodStaticField(vec2 pos) const {
	// TODO: So far, only von Neumann model is utilized.
	index_t &x = pos.x,
	        &y = pos.y;
	return Matrix<fp_t,3,3>{
		{                      0, static_field.at(y-1, x),                       0},
		{static_field.at(y, x-1),   static_field.at(y, x), static_field.at(y, x+1)},
		{                      0, static_field.at(y+1, x),                       0}
	};
}

//************************************************************

/*Matrix<bool, 3, 3> Simulation::neighbourhoodStaticField(vec2 pos) const {
	// TODO: So far, only von Neumann model is utilized.
	index_t &x = pos.x,
	        &y = pos.y;
	return Matrix<fp_t,3,3>{
		{                   0, static_field(y-1, x),                    0},
		{static_field(y, x-1),   static_field(y, x), static_field(y, x+1)},
		{                   0, static_field(y+1, x),                    0}
	};
}*/

//************************************************************

}