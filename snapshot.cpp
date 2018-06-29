#include <fstream>

#include "snapshot.h"

namespace sim{

void Snapshot::readFromFile(const std::string &filename){
	std::ifstream reader(filename, std::ios::binary);
	if(!reader){
		Output::printError("Failed to open and process file '{}'.", filename);
		throw FileFormat::FileReadException();
	}

	std::stringstream err_fname_ss;
	err_fname_ss << "Failed to process file '" << filename << "': ";
	std::string err1 = err_fname_ss.str();

	index_t dim = 0;
	std::vector<std::string> map;

	while(reader.good()){
		std::string line;
		std::getline(reader, line);

		// Ignore empty line.
		if(!line.size())
			continue;

		// CLRF correction.
		if(line[line.size()-1] == '\r')
			line.erase(line.size()-1);

		std::string token;
		std::istringstream tokenizer{line};
		while(true){
			tokenizer >> token;
			if(!tokenizer)  //end of line
				break;
			if(token[0] == '#')  //beginning of comment
				break;
			
			if(token == "dimension:"){
				tokenizer >> dim;
				if(!tokenizer){
					Output::printError(err1 + "Could not have read map dimension.");
					throw FileFormat::FileReadException();
				}

				this->dynamic_field = Field<fp_t>(dim);
				for(index_t y = 0; y != dim; ++y)
					for(index_t x = 0; x != dim; ++x)
						dynamic_field(y, x) = 0.0f;
			}
			else if(token == "map:"){
				if(!dim){
					Output::printError(err1 + "Dimension is 0 or not provided.");
					throw FileFormat::FileReadException();
				}
				// Read a number of tokens - each one represents a row of a map.
				for(index_t i = 0; i != dim; ++i){
					reader >> token;
					if(reader.fail()){
						Output::printError(err1 + "Failed to read {}. row of a map ({} expected).", i+1, dim);
						throw FileFormat::FileReadException();
					}
					if(token.size() != dim){
						Output::printError(err1 + "Row {} is of invalid length ({} expected).", i+1, dim);
						throw FileFormat::FileReadException();
					}
					map.push_back(token);
				}
			}
			else if(token == "dynamic:"){
				if(!dim){
					Output::printError(err1 + "Could not have read map dimension.");
					throw FileFormat::FileReadException();
				}

				// Read values of dynamic field.
				for(index_t y = 0; y != dim; ++y){
					// Read line.
					std::getline(reader, line);

					if(reader.fail()){
						Output::printError(err1 + "Failed to read {}. row of dynamic field ({} expected).", y+1, dim);
						throw FileFormat::FileReadException();
					}
					// Read each value.
					std::istringstream ss{line};
					for(index_t x = 0; x != dim; ++x)
						ss >> dynamic_field(y, x);
					if(ss.fail()){
						Output::printError(err1 + "Row {} is of invalid length ({} expected).", y+1, dim);
						throw FileFormat::FileReadException();
					}
				}
			}
		}
	}

	this->dimension = dim;

	for(index_t r = 0; r != dimension; ++r){
		for(index_t c = 0; c != dimension; ++c){
			vec2 pos(c, r);
			switch(tolower(map[r][c])){
			case 'p':
				this->pedestrians[UidGenerator<uid_t>::getUid()] = Pedestrian(pos);
				break;
			case 'w':
				this->walls.push_back(pos);
				break;
			case 'x':
				this->exits.push_back(pos);
				break;
			case '.':
				break;
			default:
				Output::printError(err1 + "Unknown character: '{}'", filename, map[r][c]);
			}
		}
	}

}

//************************************************************

void Snapshot::writeToFile(const std::string &filename) const {
	std::ofstream writer(filename);  //no ios::binary
	if(!writer){
		Output::printError("Failed to save a snapshot to '{}'.", filename);
		throw FileFormat::FileWriteException();
	}

	// Prepare text-mode map.
	std::vector<std::string> rows;
	rows.resize(dimension);
	for(auto &r : rows)
		r.resize(dimension, '.');
	for(const auto &p : pedestrians){
		auto pos = p.second.getPosition();
		rows[pos.y][pos.x] = 'p';
	}
	for(const auto &w : walls)
		rows[w.y][w.x] = 'W';
	for(const auto &x : exits)
		rows[x.y][x.x] = 'X';

	writer << "dimension: " << dimension << '\n';
	writer << "map:\n";
	for(const auto &r : rows)
		writer << r << '\n';
	
	// Dynamic field.
	writer << "\ndynamic:\n";
	for(index_t y = 0; y != dimension; ++y){
		for(index_t x = 0; x != dimension; ++x)
			writer << dynamic_field(y, x) << ' ';
		writer << '\n';
	}

	writer.close();
}


}