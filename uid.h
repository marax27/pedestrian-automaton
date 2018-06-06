#ifndef _UID_H_
#define _UID_H_

template<typename T>
class UidGenerator{
public:
	static T getUid(){
		return current++;
	}

	UidGenerator() = delete;

private:
	static T current;
};

template<typename T>
T UidGenerator<T>::current = T();

#endif  //_UID_H_
