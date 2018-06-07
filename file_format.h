#ifndef _FILE_FORMAT_H_
#define _FILE_FORMAT_H_

#include <string>
#include <exception>

// Abstract representation of a file format.
// Provide reading from/writing to file
// and basic exceptions.

class FileFormat{
public:
	virtual void readFromFile(const std::string &filename) = 0;
	
	virtual void writeToFile(const std::string &filename) const = 0;

	//------------------------------
	class FileReadException : public std::exception {};

	class FileWriteException : public std::exception {};
	//------------------------------
};

#endif  //_FILE_FORMAT_H_
