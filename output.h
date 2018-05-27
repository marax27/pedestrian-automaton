#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <iostream>
#include <sstream>

namespace sim{

// Signal errors and warnings.
class Output{
	typedef std::string str;

public:
	template<typename... Types>
	static void printError(
		const str &format, const Types&... args){
		std::cerr << formatString("[Error] " + format, args...) << '\n';
	}

	template<typename... Types>
	static void printWarning(
		const str &format, const Types&... args){
		std::cerr << formatString("[Warning] " + format, args...) << '\n';
	}
	
private:
	template<typename T>
	static str makeString(const T &arg)
	{
   		std::stringstream ss;
   		ss << arg;
   		return ss.str();
	}

	template<typename T>
	static str formatString(str format, const T &arg)
	{
		auto it = format.find("{}");
		if( it == str::npos )
			return format;
		return format.replace( it, 2, makeString(arg) );
	}

	template<typename T, typename... Types>
	static str formatString(str format, const T &arg, const Types&... args)
	{
		str tmp = formatString(format, arg);
		return formatString(tmp, args...);
	}
};

}

#endif  //_OUTPUT_H_
