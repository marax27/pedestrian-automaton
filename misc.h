#ifndef _MISC_H_
#define _MISC_H_

#include <cstdint>
#include <exception>

#include "output.h"

//************************************************************

template<typename T>
struct Vec2D{
	T x = T(),
	  y = T();
	
	bool operator==(const Vec2D &v) const {
		return x == v.x && y == v.y;
	}

	bool operator!=(const Vec2D &v) const {
		return !(*this == v);
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

	typedef uint32_t uid_t;
}

//************************************************************

template<typename T>
inline T squared(T x){
	return x*x;
}

template<typename T>
inline T atLeastZero(T x){
	return x < T() ? T() : x;
}

// Limit x to range [a, b].
template<typename T>
inline T limit(T a, T x, T b){
	return (x < a) ? a : (x > b ? b : x);
}

template<typename T>
T stringToValue(const std::string &s){
	std::stringstream ss{s};
	T result = T();
	ss >> result;
	if(ss.fail())
		throw std::invalid_argument("Conversion from string to value failed.");
	return result;
}

//************************************************************

#endif  //_MISC_H_
