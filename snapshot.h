#ifndef _SNAPSHOT_H_
#define _SNAPSHOT_H_

// Representation of a single step of pedestrian simulation.

// The snapshot should contain following information:
// - position of pedestrians
// - static floor field essentials: walls, exits
// - state of dynamic floor field
// - simulation constants.

#include <map>
#include <vector>
#include <string>

#include "uid.h"
#include "field.h"
#include "pedestrian.h"
#include "file_format.h"

namespace sim{

class Snapshot{
public:
	std::map<uid_t, Pedestrian> pedestrians;
	
	std::vector<vec2> walls;
	std::vector<vec2> exits;

	Field<fp_t> dynamic_field;

	index_t dimension;

	virtual void readFromFile(const std::string &filename);
	
	virtual void writeToFile(const std::string &filename) const;

private:
};

}

#endif  //_SNAPSHOT_H_