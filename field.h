#ifndef _FIELD_H_
#define _FIELD_H_

#include "misc.h"

// A 2D square field/grid of elements.

namespace sim{

template<typename T>
class Field{
public:
	typedef index_t size_type;

	Field();
	Field(size_type _size);
	Field(Field &&model);
	Field(const Field &model);

	~Field(){ free(); }

	// Assignment.

	Field& operator=(const Field &model){
		if(this == &model)
			return *this;
		free();
		size = model.size;
		copyElements(model);
		return *this;
	}

	Field& operator=(Field &&model){
		if(this == &model)
			return *this;
		free();
		data = model.data;
		size = model.size;
		model.data = nullptr;
		return *this;
	}

	// Data access.

	inline T& at(size_type column, size_type row){
		if(row >= size || column >= size){
			// throw std::out_of_range("Field::at: demanded element out of range.");
			dummy = default_dummy_state;
			return dummy;
		}
		return operator()(column, row);
	}

	inline const T& at(size_type column, size_type row) const{
		if(row >= size || column >= size){
			// throw std::out_of_range("Field::at: demanded element out of range.");
			dummy = default_dummy_state;
			return dummy;
		}
		return operator()(column, row);
	}

	inline T& operator()(size_type column, size_type row){
		return data[row*size + column];
	}

	inline const T& operator()(size_type column, size_type row) const{
		return data[row*size + column];
	}

	// Getters.
	inline size_type dimension() const { return size; }

	T sum() const {
		// Get sum of all the elements. Based on Kahan summation algorithm.
		T result = T();
		T c = T();
		for(index_t i = 0; i != size*size; ++i){
			T y = data[i] - c;
			T t = result + y;
			c = t - result - y;
			result = t;
		}
		return result;
	}

	// Setters.
	inline void setDefaultDummyState(const T &value){
		default_dummy_state = value;
	}

private:
	T *data;
	index_t size;

	// A dummy variable which is returned when out-of-range element is requested.
	mutable T dummy;
	T default_dummy_state;

	inline void free(){
		if(data){
			delete[] data;
			data = nullptr;
		}
	}

	void copyElements(const Field &model);
};

//************************************************************

template<typename T>
Field<T>::Field()
	: data(nullptr), size(0), default_dummy_state(T()) {}

//************************************************************

template<typename T>
Field<T>::Field(Field<T>::size_type _size)
	: size(_size), default_dummy_state(T()) {

	data = new T[size*size];
}

//************************************************************

template<typename T>
Field<T>::Field(Field<T> &&model)
	: data(model.data), size(model.size), default_dummy_state(T()) {

	model.data = nullptr;
}

//************************************************************

template<typename T>
Field<T>::Field(const Field<T> &model)
	: size(model.size), default_dummy_state(T()) {
		
	copyElements(model);
}

//************************************************************

// Only copy elements. Assume memory deallocation is not needed,
// as well as copying 'size' from 'model'.
template<typename T>
void Field<T>::copyElements(const Field<T> &model){
	auto count = size*size;
	data = new T[count];
	for(size_type i = 0; i != count; ++i)
		data[i] = model.data[i];
}

//************************************************************

}

#endif  //_FIELD_H_