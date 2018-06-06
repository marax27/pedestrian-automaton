#ifndef _PEDESTRIAN_H_
#define _PEDESTRIAN_H_

// A representation of a single pedestrian.

#include "misc.h"

namespace sim{

class Pedestrian{
public:
	Pedestrian() = default;
	explicit Pedestrian(vec2 pos) : position(pos) {}

	vec2 getPosition() const {
		return position;
	}

	void setPosition(vec2 new_pos) {
		position = new_pos;
	}

	void move(vec2 new_pos){
		position.x = new_pos.x;
		position.y = new_pos.y;
	}

	bool operator==(const Pedestrian &p) const {
		return position == p.position;
	}

private:
	vec2 position;
};

}

#endif  //_PEDESTRIAN_H_
