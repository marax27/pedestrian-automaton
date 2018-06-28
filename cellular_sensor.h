#ifndef _CELLULAR_SENSOR_H_
#define _CELLULAR_SENSOR_H_

#include "updateable.h"
#include "simulation.h"

#include <vector>
#include <cstdint>

namespace sim{

// Interface for sensor assigned to a single specific cell.
class CellularSensor : public Updateable, private Simulation::Viewer {
public:
	CellularSensor(vec2 pos, const Simulation &instance)
		: Simulation::Viewer(instance), position(pos) {}

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
	MotionSensor(vec2 pos, const Simulation &instance)
		: CellularSensor(pos, instance){

		if(std::max(pos.x, pos.y) >= getSnapshot().dimension)
			throw std::invalid_argument("Position of cell out of bounds.");
		previous_pedestrian = getSnapshot().pedestrians.end();
	}
	
	void update();
	void saveToFile(const std::string &filename) const;

private:
	std::vector<std::pair<index_t, uid_t>> arrivals, departures;
	std::map<uid_t, Pedestrian>::const_iterator previous_pedestrian;
};

}

#endif  //_CELLULAR_SENSOR_H_
