#ifndef _PROGRESS_BAR_H
#define _PROGRESS_BAR_H

#include <iostream>
#include <iomanip>

template<typename T> class ProgressBar;

template<typename T>
std::ostream& operator<<(std::ostream &out, const ProgressBar<T> &pbar){
	int total_len = pbar.length - 2;
	int fill_len = pbar.current * static_cast<float>(total_len) / pbar.max_val;
	// out << '[' << std::setfill('|') << std::setw(fill_len)
	    // << std::setfill('-') << std::setw(total_len - fill_len) << ']';
	out << '[' << std::string(fill_len, '|') + std::string(total_len-fill_len, ' ') << ']';
	return out;
}

template<typename T>
class ProgressBar{
public:
	typedef T value_type;
	
	ProgressBar(value_type max_value, value_type current_value = 0, uint32_t len = 10)
		: max_val(max_value), current(current_value), length(len) {
			verify();
			if(length < 3)
				length = 3;
		}

	void shift(value_type step){
		current += step;
		verify();
	}

	value_type now() const { return current; }
	value_type max() const { return max_val; }
	value_type min() const { return 0; }

private:
	value_type max_val, current;
	uint32_t length;

	void verify(){
		if(current < 0)
			current = 0;
		else if(current > max_val)
			current = max_val;
	}

	friend std::ostream& operator<< <T> (std::ostream &out, const ProgressBar &pbar);
};

#endif  //_PROGRESS_BAR_H