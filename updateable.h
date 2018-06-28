#ifndef _UPDATEABLE_H_
#define _UPDATEABLE_H_

#include <string>

// Base interface for elements which, for instance,
// are going to be updated every step.
class Updateable{
public:
	virtual void update() = 0;

	virtual void saveToFile(const std::string &filename) const = 0;
};

#endif  //_UPDATEABLE_H_
