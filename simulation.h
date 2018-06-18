#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include <algorithm>

#include "matrix.h"
#include "config.h"
#include "snapshot.h"
#include "pedestrian_priority_queue.h"

namespace sim{

// Simulation engine.
class Simulation{
public:
	Simulation() = delete;  //initial conditions required
	Simulation(const Snapshot &snapshot, const Config &configuration)
		: data(snapshot), config(configuration), time_elapsed(0) {
		initializeSimulation();
	}
	Simulation(Snapshot &&snapshot, const Config &configuration)
		: data(snapshot), config(configuration), time_elapsed(0) {
		initializeSimulation();
	}

	~Simulation() {}

	Simulation& operator=(const Simulation &s) = delete;

	// Simulation progress - 1 step per function call.
	void runStep();

	// An interface for simulation's data access.
	//------------------------------
	class Viewer{
	public:
		Viewer(const Simulation &instance) : target(instance) {}

		const std::map<uid_t, Pedestrian>& getPedestrians() const {
			return target.data.pedestrians;
		}

		const std::vector<vec2>& getWalls() const {
			return target.data.walls;
		}

		const std::vector<vec2>& getExits() const {
			return target.data.exits;
		}

		const Field<fp_t>& getDynamicField() const {
			return target.data.dynamic_field;
		}

		index_t getDimension() const {
			return target.data.dimension;
		}

		index_t getTime() const {
			return target.time_elapsed;
		}

		const Field<fp_t>& getStaticField() const {
			return target.static_field;
		}

	private:
		const Simulation &target;
	};
	//------------------------------
	class SimulationException : public std::exception {};
	//------------------------------

private:
	Snapshot data;
	Config config;
	index_t time_elapsed;

	// Calculated once during initialization of a Simulation.
	Field<fp_t> static_field;

	PedestrianPriorityQueue<fp_t> *pqueue;

	void initializeSimulation(){
		// Prepare the static field.
		initializeStaticField();

		// Construct the priority queue.
		pqueue = new PedestrianPriorityQueue<fp_t>(
			static_field, data.pedestrians);
		
		// Fill the priority queue.
		for(const auto &i : data.pedestrians)
			pqueue->push(i.first);
	}

	void initializeStaticField();

	Matrix<fp_t, 3, 3> neighbourhoodStaticField(vec2 pos) const;
	Matrix<fp_t, 3, 3> neighbourhoodDynamicField(vec2 pos) const;
	Matrix<bool, 3, 3> neighbourhoodOccupiedFields(vec2 pos) const;

	std::map<uid_t, Pedestrian>::const_iterator getPedestrianByPos(vec2 pos) const;	
	bool occupiedByPedestrian(vec2 pos) const;

	inline bool isExit(vec2 pos) const {
		return std::find(data.exits.begin(), data.exits.end(), pos)
				!= data.exits.end();
	}

	inline bool isWall(vec2 pos) const {
		return std::find(data.walls.begin(), data.walls.end(), pos)
				!= data.walls.end();
	}

	// Get usable value of dynamic field at given position.
	// Note: usable value != raw dynamic field value
	// 0 <= Dij < +inf
	// 0 <= dynamicValue < x
	fp_t dynamicValue(vec2 pos) const;

	friend class Viewer;
};

}

#endif  //_SIMULATION_H_