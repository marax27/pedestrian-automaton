#ifndef _UPDATEABLE_H_
#define _UPDATEABLE_H_
#define _UPDATEABLE_H_

// Base interface for elements which, for instance,
// are going to be updated every step.
class Updateable{
public:
	virtual void update() = 0;
};

#endif  //_UPDATEABLE_H_
