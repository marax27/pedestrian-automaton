#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include "snapshot.h"

namespace sim{

// Simulation engine.
class Simulation{
public:
	Simulation() = delete;  //initial conditions required
	Simulation(const Snapshot &snapshot) : data(snapshot), time_elapsed(0) {
		initializeStaticField();
	}
	Simulation(Snapshot &&snapshot) : data(snapshot), time_elapsed(0) {
		initializeStaticField();
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

		const std::vector<Pedestrian>& getPedestrians() const {
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

	void initializeStaticField();

	friend class Viewer;
};

}

#endif  //_SIMULATION_H_