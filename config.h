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

	fp_t decay = 0.25,
	     max = 1.0,
	     min = 0.0;
	
	e_NeighbourhoodType neighbourhood;

};

}

#endif  //_CONFIG_H_
