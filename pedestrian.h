#ifndef _PEDESTRIAN_H_
#define _PEDESTRIAN_H_

// A representation of a single pedestrian.

#include "misc.h"
#include "happiness.h"

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

	void increaseHappiness(){ happiness.increase(); }
	void decreaseHappiness(){ happiness.decrease(); }
	bool isHappy() const { return happiness.isHappy(); }

	bool operator==(const Pedestrian &p) const {
		return position == p.position;
	}

private:
	vec2 position;
	happy_t happiness;
};

}

#endif  //_PEDESTRIAN_H_
