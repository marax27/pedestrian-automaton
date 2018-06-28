#ifndef _HAPPINESS_H_
#define _HAPPINESS_H_

#include <cstdint>

namespace sim{

// Happiness.
class HappinessMeasurement{
	const static int8_t MIN_STATE;
	const static int8_t MAX_STATE;
	int8_t counter;
public:
	HappinessMeasurement() : counter(MAX_STATE) {}

	void decrease(){
		if(counter > 0)
			--counter;
		else
			counter = MIN_STATE;
	}
	
	void increase(){
		if(counter < 0)
			++counter;
		else
			counter = MAX_STATE;
	}

	bool isHappy() const { return counter > 0; }
};
	
typedef HappinessMeasurement happy_t;

}

#endif  //_HAPPINESS_H_
