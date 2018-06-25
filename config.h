#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <exception>

#include "misc.h"
#include "file_format.h"

namespace sim{

class Config{
public:
	enum e_NeighbourhoodType {
		VON_NEUMANN, MOORE
	};

	// Constants.

	fp_t static_decay = 0.25,
	     max = 1.0,
	     min = 0.0,
		 static_strength = 1.0;
	
	e_NeighbourhoodType neighbourhood;

	fp_t dynamic_usable_decay = 0.25,
	     dynamic_usable_max = 1.0;
	
	fp_t dynamic_step = 1.0,
	     dynamic_decay = 0.1,
	     diffusion = 0.9,
	     dynamic_strength = 1.0;

	virtual void readFromFile(const std::string &filename);
	
	virtual void writeToFile(const std::string &filename) const;
};

}

#endif  //_CONFIG_H_
