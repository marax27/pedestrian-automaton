#ifndef _HAPPINESS_H_
#define _HAPPINESS_H_

#include <cstdint>

namespace sim{

// Happiness.
class HappinessMeasurement{
	// Scale of happiness:
	// -2 -1 0 1 2 3
	// unhappy happy
	int8_t counter;
public:
	HappinessMeasurement() : counter(2) {}

	void decrease(){
		if(counter > -2)
			--counter;
	}
	
	void increase(){
		if(counter < 3)
			++counter;
	}

	bool isHappy() const { return counter > 0; }
};
	
typedef HappinessMeasurement happy_t;

}

#endif  //_HAPPINESS_H_
