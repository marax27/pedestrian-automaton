#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include "matrix.h"
#include "snapshot.h"
#include "pedestrian_priority_queue.h"

namespace sim{

// Simulation engine.
class Simulation{
public:
	Simulation() = delete;  //initial conditions required
	Simulation(const Snapshot &snapshot)
		: data(snapshot), time_elapsed(0) {
		initializeSimulation();
	}
	Simulation(Snapshot &&snapshot)
		: data(snapshot), time_elapsed(0) {
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
		Viewer(Simulation &instance) : target(instance) {}

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
		Simulation &target;
	};
	//------------------------------

private:
	Snapshot data;
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

	friend class Viewer;
};

}

#endif  //_SIMULATION_H_