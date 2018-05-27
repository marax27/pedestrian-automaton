#ifndef _MISC_H_
#define _MISC_H_

#include <cstdint>

#include "output.h"

//************************************************************

template<typename T>
struct Vec2D{
	T x = T(),
	  y = T();
};

//************************************************************

namespace sim{
	// sim::index_t should be used e.g. while iterating over
	// simulation's cells.
	typedef uint32_t index_t;

	// Floating-point type used in computations throughout the program.
	// In case float is not enough, or double is too much.
	typedef float fp_t; 

	typedef Vec2D<index_t> vec2;
}

//************************************************************

#endif  //_MISC_H_
