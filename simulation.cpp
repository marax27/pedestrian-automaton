#include <algorithm>
#include <limits>
#include <cmath>

#include "matrix.h"
#include "simulation.h"

// Return random element of a matrix. Treat values as weights.
sim::vec2 randomMatrixElement(const Matrix<sim::fp_t, 3, 3> &m);

namespace sim{

//************************************************************

void Simulation::runStep(){
	// TODO: Utilize and update dynamic field.

	auto *new_queue = new PedestrianPriorityQueue<fp_t>(static_field, data.pedestrians);

	while(!pqueue->empty()){
		uid_t id = pqueue->top();
		vec2 pos = data.pedestrians.at(id).getPosition();

		// Get values of static field in the neighbourhood of a pedestrian.
		Matrix<fp_t, 3, 3> S = neighbourhoodStaticField(pos);  //std::move maybe?

		// Get occupation matrix.
		Matrix<bool, 3, 3> N = neighbourhoodOccupiedFields(pos);

		// TODO: dynamic field.

		/*std::cerr << '(' << pos.x << ", " << pos.y << ")\n";
		for(int y=0; y!=3; ++y){
			for(int x=0; x!=3; ++x)
				std::cerr << N(y, x) << ' ';
			std::cerr << "   ";
			for(int x=0; x!=3; ++x)
				std::cerr << S(y,x) << ' ';
			std::cerr << '\n';
		}
		std::cerr << '\n';*/

		// Probability matrix.
		Matrix<fp_t, 3, 3> P = S * N.as<fp_t>();

		vec2 new_pos = randomMatrixElement(P);
		// Coords of new_pos contains values from {0, 1, 2},
		// which represent pedestrian's offset:  {-1, 0, 1}.
		// Since index_t is unsigned, one should avoid diving below 0.
		pos = {pos.x + new_pos.x - 1, pos.y + new_pos.y - 1};
		data.pedestrians.at(id).setPosition(pos);

		pqueue->pop();
		if(isExit(pos)){
			// Remove pedestrian from data.pedestrians and do not add
			// push him/her into the queue.
			data.pedestrians.erase(getPedestrianByPos(pos));
		}
		else
			new_queue->push(id);
	}

	delete pqueue;
	pqueue = new_queue;

	++time_elapsed;
}

//************************************************************

void Simulation::initializeStaticField(){
	// Exits: 1.0
	// Free space: A*e^(-ax) + min
	// Walls: 0.0

	const fp_t EXIT = config.max,
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
			auto A = config.max - config.min;
			static_field(y, x) = A * exp(-config.decay*min_dist) + config.min;
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
	index_t &x = pos.x,
	        &y = pos.y;

	switch(config.neighbourhood){
		case Config::VON_NEUMANN:
		return Matrix<fp_t,3,3>{
		 {                      0, static_field.at(x, y-1),                       0},
		 {static_field.at(x-1, y),   static_field.at(x, y), static_field.at(x+1, y)},
		 {                      0, static_field.at(x, y+1),                       0}
		};
		case Config::MOORE:
		return  Matrix<fp_t,3,3>{
		 {static_field.at(x-1, y-1), static_field.at(x, y-1), static_field.at(x+1, y-1)},
		 {  static_field.at(x-1, y),   static_field.at(x, y), static_field.at(x+1, y)},
		 {static_field.at(x-1, y+1), static_field.at(x, y+1), static_field.at(x+11, y+1)}
		};
		default:
			throw SimulationException();
	}
}

//************************************************************

Matrix<bool, 3, 3> Simulation::neighbourhoodOccupiedFields(vec2 pos) const {
	// TODO: So far, only von Neumann model is utilized.
	index_t &x = pos.x,
	        &y = pos.y;

	switch(config.neighbourhood){
		case Config::VON_NEUMANN:
		return Matrix<bool,3,3>{
		 {                              0, !occupiedByPedestrian({x, y-1}), 0},
		 {!occupiedByPedestrian({x-1, y}),                               1, !occupiedByPedestrian({x+1, y})},
		 {                              0, !occupiedByPedestrian({x, y+1}), 0}
		};
		case Config::MOORE:
		return Matrix<bool,3,3>{
		 {!occupiedByPedestrian({x-1,y-1}), !occupiedByPedestrian({x,y-1}), !occupiedByPedestrian({x+1,y-1}),},
		 {!occupiedByPedestrian({x-1,y}),                                1, !occupiedByPedestrian({x+1, y})},
		 {!occupiedByPedestrian({x-1,y+1}), !occupiedByPedestrian({x,y+1}), !occupiedByPedestrian({x+1,y+1}),}
		};
		default:
			throw SimulationException();
	}
}

//************************************************************

std::map<uid_t, Pedestrian>::const_iterator
	Simulation::getPedestrianByPos(vec2 pos) const {
	
	const auto it = std::find_if(
		data.pedestrians.begin(), data.pedestrians.end(),
		[&pos](const std::pair<uid_t,Pedestrian> &p){
			return p.second.getPosition() == pos;
		}
	);
	return it;
}

//************************************************************

bool Simulation::occupiedByPedestrian(vec2 pos) const {
	return getPedestrianByPos(pos) != data.pedestrians.end();
}

//************************************************************

}

//************************************************************

sim::vec2 randomMatrixElement(const Matrix<sim::fp_t, 3, 3> &m){
	sim::fp_t sum = 0.0, t = 0.0;
	for(sim::index_t i = 0; i != 9; ++i)
		sum += m((i - i%3)/3, i%3);

	sim::fp_t random = (double)rand() / RAND_MAX;  // 0 < random < 1
	random *= sum;

	for(sim::index_t i = 0; i != 9; ++i){
		t += m((i - i%3)/3, i%3);
		if(random <= t)
			return sim::vec2(i%3, (i - i%3)/3);
	}
	// Perhaps throw something?
	return sim::vec2(2, 2);
}

//************************************************************