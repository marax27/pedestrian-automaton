#ifndef _MISC_H_
#define _MISC_H_

#include <cstdint>

#include "output.h"

//************************************************************

template<typename T>
struct Vec2D{
	T x = T(),
	  y = T();
	
	bool operator==(const Vec2D &v) const {
		return x == v.x && y == v.y;
	}

	Vec2D() = default;
	Vec2D(T _x, T _y) : x(_x), y(_y) {}
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

template<typename T>
inline T squared(T x){
	return x*x;
}

//************************************************************

#endif  //_MISC_H_
