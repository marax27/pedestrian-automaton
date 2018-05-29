#ifndef _SNAPSHOT_H_
#define _SNAPSHOT_H_

// Representation of a single step of pedestrian simulation.

// The snapshot should contain following information:
// - position of pedestrians
// - static floor field essentials: walls, exits
// - state of dynamic floor field
// - simulation constants.

#include <vector>
#include <string>

#include "field.h"
#include "pedestrian.h"

namespace sim{

class Snapshot{
public:
	std::vector<Pedestrian> pedestrians;
	
	std::vector<vec2> walls;
	std::vector<vec2> exits;

	Field<fp_t> dynamic_field;

	index_t dimension;

	void readFromFile(const std::string &filename);
	
	void writeToFile(const std::string &filename) const;

	//------------------------------
	class FileReadException : public std::exception {};

	class FileWriteException : public std::exception {};
	//------------------------------

private:
	
};

}

#endif  //_SNAPSHOT_H_