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

		// Get dynamic field.
		Matrix<fp_t, 3, 3> D = neighbourhoodDynamicField(pos);

		// Probability matrix.
		Matrix<fp_t, 3, 3> P = S * N.as<fp_t>() * D;

		/*std::cerr << '(' << pos.x << ", " << pos.y << ")\n";
		for(int y=0; y!=3; ++y){
			for(int x=0; x!=3; ++x)
				std::cerr << D(x,y) << ' ';
			std::cerr << '\n';
		}
		std::cerr << '\n';*/

		vec2 new_pos = randomMatrixElement(P);

		// Coords of new_pos contains values from {0, 1, 2},
		// which represent pedestrian's offset:  {-1, 0, 1}.
		// Since index_t is unsigned, one should avoid diving below 0.
		pos = {pos.x + new_pos.x - 1, pos.y + new_pos.y - 1};
		data.pedestrians.at(id).setPosition(pos);

		// Determine if step should increase/decrease happiness.
		// 1st approach: compare value of dynamic field in new_pos
		// with average value of P.
		// Assume that not moving at all lowers happiness.
		fp_t sum = 0.0f, avg;
		int n = 0;
		vec2 current_pos;
		for(index_t i = 0; i != 9; ++i){
			index_t _x = i%3, _y = (i - i%3)/3;

			// Take into account fields that are unoccupied.
			if(S(_x,_y) && N(_x,_y) && vec2(_x,_y) != vec2(1,1)){
				sum += P(current_pos.x, current_pos.y);
				++n;
			}
		}
		if(!n)  //no way to go
			avg = std::numeric_limits<fp_t>::infinity();
		else
			avg = sum / n;
		
		// Maybe '>= avg'?
		if(P(new_pos.x, new_pos.y) > avg && new_pos != vec2(1,1))
			data.pedestrians[id].increaseHappiness();  //increase happiness.
		else
			data.pedestrians[id].decreaseHappiness();  //decrease happiness.

		// If pedestrian's happy, increment dynamic field
		// of a cell now occupied by them.
		if(data.pedestrians[id].isHappy()){
			auto &cell = data.dynamic_field.at(pos.x, pos.y);
			cell += config.dynamic_step;
		}

		pqueue->pop();
		if(isExit(pos)){
			// Remove pedestrian from data.pedestrians and do not add
			// push him/her into the queue.
			data.pedestrians.erase(getPedestrianByPos(pos));
		}
		else
			new_queue->push(id);
	}

	// Perform diffusion and decay of dynamic field.
	for(index_t y = 0; y != data.dynamic_field.dimension(); ++y){
		for(index_t x = 0; x != data.dynamic_field.dimension(); ++x){
			fp_t delta = config.dynamic_decay * data.dynamic_field(x, y);

			// Decay.
			data.dynamic_field(x, y) -= delta;

			// Diffusion.
			int rnd;
			vec2 shifted;
			do{
				rnd = rand() % 9;  // Picking random neighbour.
				                   // Picking itself is also possible.
				shifted = {x + rnd%3 - 1, y + (rnd - rnd%3)/3 - 1};
			}while(isWall(shifted));  //Do not diffuse into walls.

			data.dynamic_field.at(shifted.x, shifted.y) += delta;
		}
	}

	// Compute average value of dynamic field.
	avg_dynamic_field = data.dynamic_field.sum() / (data.dimension * data.dimension);

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
			static_field(x, y) = A * exp(-config.static_decay*min_dist) + config.min;
		}
	}

	// Assign appropriate special values to walls and exits.
	for(const auto &ex : data.exits)
		static_field(ex.x, ex.y) = EXIT;
	for(const auto &w : data.walls)
		static_field(w.x, w.y) = WALL;
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

Matrix<fp_t, 3, 3> Simulation::neighbourhoodDynamicField(vec2 pos) const {
	index_t &x = pos.x,
	        &y = pos.y;

	switch(config.neighbourhood){
		case Config::VON_NEUMANN:
		return Matrix<fp_t,3,3>{
		 {                     0, dynamicValue({x, y-1}), 0},
		 {dynamicValue({x-1, y}),   dynamicValue({x, y}), dynamicValue({x+1, y})},
		 {                     0, dynamicValue({x, y+1}), 0}
		};
		case Config::MOORE:
		return  Matrix<fp_t,3,3>{
		 {dynamicValue({x-1, y-1}), dynamicValue({x, y-1}), dynamicValue({x+1, y-1})},
		 {  dynamicValue({x-1, y}), dynamicValue({x, y}),   dynamicValue({x+1, y})},
		 {dynamicValue({x-1, y+1}), dynamicValue({x, y+1}), dynamicValue({x+1, y+1})}
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

fp_t Simulation::dynamicValue(vec2 pos) const {
	if(pos.x >= data.dynamic_field.dimension() || pos.y >= data.dynamic_field.dimension())
		return 0.0;
	return config.dynamic_usable_max * (
		1 - exp(-config.dynamic_usable_decay * abs(data.dynamic_field(pos.x, pos.y) - avg_dynamic_field) )
	) + 1e-10;
}

//************************************************************

}

//************************************************************

sim::vec2 randomMatrixElement(const Matrix<sim::fp_t, 3, 3> &m){
	sim::fp_t sum = 0.0, t = 0.0;
	for(sim::index_t i = 0; i != 9; ++i)
		sum += m(i%3, (i - i%3)/3);

	sim::fp_t random = (double)rand() / RAND_MAX;  // 0 < random < 1
	random *= sum;

	for(sim::index_t i = 0; i != 9; ++i){
		t += m(i%3, (i - i%3)/3);
		if(random <= t)
			return sim::vec2(i%3, (i - i%3)/3);
	}
	// Perhaps throw something?
	return sim::vec2(2, 2);
}

//************************************************************