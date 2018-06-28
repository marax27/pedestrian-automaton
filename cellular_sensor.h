#ifndef _CELLULAR_SENSOR_H_
#define _CELLULAR_SENSOR_H_

#include "updateable.h"
#include "simulation.h"
#include "chart.h"

#include <vector>
#include <cstdint>

namespace sim{

// Interface for sensor assigned to a single specific cell.
class CellularSensor : public Updateable, private Simulation::Viewer {
public:
	CellularSensor(const Simulation &instance, vec2 pos)
		: Simulation::Viewer(instance), position(pos) {
		
		if(std::max(pos.x, pos.y) >= getSnapshot().dimension)
			throw std::invalid_argument("Position of cell out of bounds.");
	}

protected:
	using Simulation::Viewer::getSnapshot;
	using Simulation::Viewer::getTime;

	vec2 position;
	// const Snapshot &snapshot;
};



// Detects and saves information about pedestrians who enter or exit
// a specified cell.
class MotionSensor : public CellularSensor {
public:
	MotionSensor(const Simulation &instance, vec2 pos)
		: CellularSensor(instance, pos){

		previous_pedestrian = getSnapshot().pedestrians.end();
	}
	
	void update();
	void saveToFile(const std::string &filename) const;

private:
	std::vector<std::pair<index_t, uid_t>> arrivals, departures;
	std::map<uid_t, Pedestrian>::const_iterator previous_pedestrian;
};



// Sensor which measures value of dynamic field over time.
class DynamicSensor : public CellularSensor, public Chart {
public:
	DynamicSensor(const Simulation &instance, vec2 pos, Colour c)
		: CellularSensor(instance, pos), Chart(instance, c) {}
	
	void update();
	void saveToFile(const std::string &filename) const;

private:
	std::vector<fp_t> data;
};

}

#endif  //_CELLULAR_SENSOR_H_
